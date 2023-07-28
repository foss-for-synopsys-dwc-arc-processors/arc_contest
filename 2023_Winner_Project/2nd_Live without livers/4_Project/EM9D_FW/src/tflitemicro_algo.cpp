/*
 * tflitemicro_algo.cc
 *
 *  Created on: 2020�~5��27��
 *      Author: 902447
 */

/*
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "embARC.h"
#include "embARC_debug.h"
#include "embARC_error.h"
#include "board_config.h"
#include "arc_timer.h"
*/

#include <time.h>

#include <library/cv/tflitemicro_24/tensorflow/lite/c/common.h>
#include <library/cv/tflitemicro_24/tensorflow/lite/micro/kernels/micro_ops.h>
#include <library/cv/tflitemicro_24/tensorflow/lite/micro/micro_error_reporter.h>
#include <library/cv/tflitemicro_24/tensorflow/lite/micro/micro_interpreter.h>
#include <library/cv/tflitemicro_24/tensorflow/lite/micro/micro_mutable_op_resolver.h>
//#include <library/cv/tflitemicro_24/tensorflow/lite/micro/testing/micro_test.h>
#include <library/cv/tflitemicro_24/tensorflow/lite/schema/schema_generated.h>
#include <library/cv/tflitemicro_24/tensorflow/lite/version.h>

#include "tflitemicro_algo.h"
#include "model_settings.h"
#include "model.h"

constexpr int tensor_arena_size = 125*1024 * 3 + 45 * 1024;
#ifdef __GNU__
	uint8_t tensor_arena[tensor_arena_size] __attribute__((aligned(16)));
#else
	#pragma Bss(".tensor_arena")
	uint8_t tensor_arena[tensor_arena_size];
	#pragma Bss()
#endif

// for WOLO
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

const float iou_thres = 0.45;

// pred = xyxy, conf, dist, cls
float pred[max_bbox][7] = {0};
int8_t pred_idx;

void addBox(float x, float y, float w, float h, float conf, float dist, float cls){
	float x1 = x - w/2;
	float y1 = y - h/2;
	float x2 = x + w/2;
	float y2 = y + h/2;

	// NMS
	float lowest_conf = 0;
	int8_t lowest_conf_idx = -1;
	for(int i=0;i<pred_idx;i++){
		float ix1 = MAX(x1, pred[i][0]);
		float iy1 = MAX(y1, pred[i][1]);
		float ix2 = MIN(x2, pred[i][2]);
		float iy2 = MIN(y2, pred[i][3]);

		float intersection = MAX(0, ix2-ix1) * MAX(0, iy2-iy1);
		float area1 = (x2 - x1) * (y2 - y1);
		float area2 = (pred[i][2] - pred[i][0]) * (pred[i][3] - pred[i][1]);

		float iou = (float)intersection / float(area1 + area2 - intersection);

		if(iou > iou_thres){
			if(conf > pred[i][4]){
				// replace
				pred[i][0] = x1;
				pred[i][1] = y1;
				pred[i][2] = x2;
				pred[i][3] = y2;
				pred[i][4] = conf;
				pred[i][5] = dist;
				pred[i][6] = cls;
				return ;
			}
			else{
				// same box with lower conf. drop it
				return ;
			}
		}

		// update lowest conf
		if(pred[i][4] < lowest_conf){
			lowest_conf = pred[i][4];
			lowest_conf_idx = i;
		}
	}

	// add new bbox
	if(pred_idx == max_bbox){
		// replace with lowest conf bbox
		pred[lowest_conf_idx][0] = x1;
		pred[lowest_conf_idx][1] = y1;
		pred[lowest_conf_idx][2] = x2;
		pred[lowest_conf_idx][3] = y2;
		pred[lowest_conf_idx][4] = conf;
		pred[lowest_conf_idx][5] = dist;
		pred[lowest_conf_idx][6] = cls;
	}
	else{
		// append to the pred list
		
		pred[pred_idx][0] = x1;
		pred[pred_idx][1] = y1;
		pred[pred_idx][2] = x2;
		pred[pred_idx][3] = y2;
		pred[pred_idx][4] = conf;
		pred[pred_idx][5] = dist;
		pred[pred_idx][6] = cls;
		
		pred_idx++;
	}
}

void printPred(){
	for(int i=0;i<pred_idx;i++){
		printf(	"Detect object idx = %d, x1=%d, y1=%d, x2=%d, y2=%d, conf=%d, dist=%d \n",
				(int)pred[i][6], (int)pred[i][0], (int)pred[i][1], (int)pred[i][2], (int)pred[i][3], (int)(pred[i][4]*100), (int)pred[i][5]);
	}
}


// end WOLO




namespace {
tflite::MicroErrorReporter micro_error_reporter;
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output1 = nullptr;
}

//TFu debug log, make it hardware platform print
extern "C" void DebugLog(const char* s) {xprintf("%s", s);}//{ fprintf(stderr, "%s", s); }

extern "C" int tflitemicro_algo_init()
{
	pred_idx = 0;

	int ercode = 0;
	TfLiteStatus ret;

	error_reporter = &micro_error_reporter;

	error_reporter->Report("TFLM model setting\n");
	model = ::tflite::GetModel(my_model_tflite);
	if (model->version() != TFLITE_SCHEMA_VERSION) {
		error_reporter->Report(
			"Model provided is schema version %d not equal "
			"to supported version %d.\n",
			model->version(), TFLITE_SCHEMA_VERSION);
	}


	static tflite::MicroMutableOpResolver<8> micro_op_resolver;
	micro_op_resolver.AddConv2D();
	micro_op_resolver.AddRelu6();
	micro_op_resolver.AddMaxPool2D();
	micro_op_resolver.AddPad();
	micro_op_resolver.AddConcatenation();
	micro_op_resolver.AddResizeNearestNeighbor();
	micro_op_resolver.AddDequantize();
	micro_op_resolver.AddQuantize();


	// Build an interpreter to run the model with.
	static tflite::MicroInterpreter static_interpreter(model, micro_op_resolver,
										   tensor_arena, tensor_arena_size,
										   error_reporter);
	interpreter = &static_interpreter;
	if(interpreter->AllocateTensors()!= kTfLiteOk) {
		ercode = -1;
	}

	input = interpreter->input(0);
	output1 = interpreter->output(0);
	return ercode;
}

extern "C" int tflitemicro_algo_run(uint8_t * ptr_image)
{
	printf("CLOCKS PRE SEC = %d\n", CLOCKS_PER_SEC);
	clock_t begin = clock();

	int ercode = 0;
	//zero padding for top
	for(int i=0; i<8*320;i++)
		input->data.f[i] = 0;
	
	// input image
    for (int i = 8*320; i < kImageSize + 8*320; i++) 
      input->data.f[i] = *(ptr_image + i) / 255.0;

	//zero padding for bottom
	for(int i=kImageSize + 8*320;i<320*256;i++)
		input->data.f[i] = 0;

	TfLiteStatus invoke_status = interpreter->Invoke();

	if(invoke_status != kTfLiteOk)
	{
		error_reporter->Report("invoke fail\n");
	}
	
	float* results_ptr = output1->data.f; 

	//**********************************//
	//				IDetect				//
	//**********************************//
	// Hyperparameter
	const float conf_thres = 0.25; 
	const int8_t bias = 0;
	const float quantize = 1;



	// stride = 32
	// grid = 8,13 31,15 68,45
	int8_t anchor_grid[3][2] = {
		{12,23},
		{45,22},
		{80,60}
	};
	// 8x10 * xywh conf dist (conf of cls )*4
	int8_t anchor_grid_idx = -1;
	int8_t gridx = 0;
	int8_t gridy = 0;

	for(int i=0;i<2400;i+=10){

		anchor_grid_idx++;
		if(anchor_grid_idx == 3){
			anchor_grid_idx = 0;
			gridx++;
			if(gridx == 10){
				gridx = 0;
				gridy++;
			}
		}
		
		// confident
		float conf = (*(results_ptr + i + 4));
		conf = 1.0 / (1.0 + exp(-conf));

		float best_conf = 0;
		float best_cls = -1;
		for(int j=0;j<4;j++){
			float cls_conf = (*(results_ptr + i + 6 + j));
			cls_conf = 1.0 / (1.0 + exp(-cls_conf));
			cls_conf = cls_conf * conf;
			if(cls_conf > conf_thres && cls_conf > best_conf){
				best_conf = cls_conf;
				best_cls = j;
			}
		
		}
		if(best_conf < conf_thres)
			continue;
		// printf("best conf = %f, detect : %d", best_conf, best_cls);
		
		// distance
		float dist = float(*(results_ptr + i + 5));
		dist *= 30;

		// dequantize
		float x = float(*(results_ptr + i));
		float y = float(*(results_ptr + i + 1));
		float w = float(*(results_ptr + i + 2));
		float h = float(*(results_ptr + i + 3));

		// sigmoid
		x = 1.0 / (1.0 + exp(-x));
		y = 1.0 / (1.0 + exp(-y));
		w = 1.0 / (1.0 + exp(-w));
		h = 1.0 / (1.0 + exp(-h));

		// final result
		x = (x * 2 - 0.5 + gridx) * 32;
		y = (y * 2 - 0.5 + gridy) * 32;
		w = pow((w * 2),2) * anchor_grid[anchor_grid_idx][0];
		h = pow((h * 2),2) * anchor_grid[anchor_grid_idx][1];

		if( y < 8 || y > 248)
			continue;
		addBox(x, y, w, h, best_conf, dist, best_cls);
	}

	// printPred();

	printf("run time = %d\n", (int)(double(clock() - begin)));
	return ercode;
}

