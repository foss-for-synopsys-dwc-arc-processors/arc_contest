/*
 * tflitemicro_algo.cc
 *
 *  Created on: 2020�~5��27��
 *      Author: 902447
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "embARC.h"
#include "embARC_debug.h"
#include "embARC_error.h"
#include "board_config.h"
#include "arc_timer.h"


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
#include "model_2.h"

constexpr int tensor_arena_size = 128*1024; //128*1024;
#ifdef __GNU__
	uint8_t tensor_arena[tensor_arena_size] __attribute__((aligned(16)));
#else
	#pragma Bss(".tensor_arena")
	uint8_t tensor_arena[tensor_arena_size];
	#pragma Bss()
#endif


constexpr int tensor_arena_size_2 = 128*1024; //128*1024;
#ifdef __GNU__
	uint8_t tensor_arena_2[tensor_arena_size_2] __attribute__((aligned(16)));
#else
	#pragma Bss(".tensor_arena")
	uint8_t tensor_arena_2[tensor_arena_size_2];
	#pragma Bss()
#endif

namespace {
tflite::MicroErrorReporter micro_error_reporter;
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
////////////////////////////////////////////////////
tflite::MicroErrorReporter micro_error_reporter_2;
tflite::ErrorReporter* error_reporter_2 = nullptr;
const tflite::Model* model_2 = nullptr;
tflite::MicroInterpreter* interpreter_2 = nullptr;
TfLiteTensor* input_2 = nullptr;
TfLiteTensor* output_2 = nullptr;
}

//TFu debug log, make it hardware platform print
extern "C" void DebugLog(const char* s) {xprintf("%s", s);}//{ fprintf(stderr, "%s", s); }

extern "C" int tflitemicro_algo_init()
{
	int ercode = 0;
	TfLiteStatus ret;

	error_reporter = &micro_error_reporter;

	error_reporter->Report("TFLM model setting\n");
	model = ::tflite::GetModel(model_tflite);
	if (model->version() != TFLITE_SCHEMA_VERSION) {
		error_reporter->Report(
			"Model provided is schema version %d not equal "
			"to supported version %d.\n",
			model->version(), TFLITE_SCHEMA_VERSION);
	}


	// static tflite::MicroMutableOpResolver<5> micro_op_resolver;
	// micro_op_resolver.AddConv2D();
	// micro_op_resolver.AddMaxPool2D();
	// micro_op_resolver.AddFullyConnected();
	// micro_op_resolver.AddReshape();
	// micro_op_resolver.AddSoftmax();
	static tflite::MicroMutableOpResolver<7> micro_op_resolver;
	micro_op_resolver.AddConv2D();
	micro_op_resolver.AddMaxPool2D();
	micro_op_resolver.AddFullyConnected();
	micro_op_resolver.AddRelu();
	micro_op_resolver.AddReshape();
	micro_op_resolver.AddLogistic();
	micro_op_resolver.AddDequantize(); 






	// Build an interpreter to run the model with.
	static tflite::MicroInterpreter static_interpreter(model, micro_op_resolver,
										   tensor_arena, tensor_arena_size,
										   error_reporter);
	interpreter = &static_interpreter;
	if(interpreter->AllocateTensors()!= kTfLiteOk) {
		ercode = -1;
	}

	input = interpreter->input(0);
	output = interpreter->output(0);

	return ercode;
}

extern "C" float tflitemicro_algo_run(float * ptr_image)//int
{
	int ercode = 0;
	//printf("model check1\n");
	for (int i = 0; i < kImageSize; i++) 
	{
  		input->data.f[i] = *(ptr_image + i);
	}

	//printf("model check2\n");
	TfLiteStatus invoke_status = interpreter->Invoke();
	//printf("model check3\n");
	if(invoke_status != kTfLiteOk)
	{
		error_reporter->Report("invoke fail\n");
		printf("invoke fail\n");
		//return -1;
	}
	
	//int8_t* results_ptr = output->data.int8;
	//float* results_ptr = output->data.f;
	//printf("model check1\n");
	TfLiteTensor* output = interpreter->output(0);
	//printf("model check2\n");
	float* results_ptr = output->data.f;
	//printf("model check3\n");
	float result = results_ptr[0];
	//printf("model check4\n");
    //int result = std::distance(results_ptr, std::max_element(results_ptr, results_ptr + 26));
	//float result = interpreter->output(0)->data.f[0];
	//float probability = std::round(result * 100.0);

	//printf("model check3\n");
	//ercode = result;

	return result;//return ercode
}

///--OOO

extern "C" int tflitemicro_algo_init_2()
{
	int ercode = 0;
	TfLiteStatus ret;

	error_reporter_2 = &micro_error_reporter_2;

	error_reporter_2->Report("TFLM model 2 setting\n");
	model_2 = ::tflite::GetModel(model_tflite_2);
	if (model_2->version() != TFLITE_SCHEMA_VERSION) {
		error_reporter_2->Report(
			"Model_2 provided is schema version %d not equal "
			"to supported version %d.\n",
			model_2->version(), TFLITE_SCHEMA_VERSION);
	}


	// static tflite::MicroMutableOpResolver<5> micro_op_resolver;
	// micro_op_resolver.AddConv2D();
	// micro_op_resolver.AddMaxPool2D();
	// micro_op_resolver.AddFullyConnected();
	// micro_op_resolver.AddReshape();
	// micro_op_resolver.AddSoftmax();
	static tflite::MicroMutableOpResolver<7> micro_op_resolver;
	micro_op_resolver.AddConv2D();
	micro_op_resolver.AddMaxPool2D();
	micro_op_resolver.AddFullyConnected();
	micro_op_resolver.AddRelu();
	micro_op_resolver.AddReshape();
	micro_op_resolver.AddLogistic();
	micro_op_resolver.AddDequantize(); 






	// Build an interpreter to run the model with.
	static tflite::MicroInterpreter static_interpreter(model_2, micro_op_resolver,
										   tensor_arena_2, tensor_arena_size_2,
										   error_reporter_2);
	interpreter_2 = &static_interpreter;
	if(interpreter_2->AllocateTensors()!= kTfLiteOk) {
		ercode = -1;
	}

	input_2 = interpreter_2->input(0);
	output_2 = interpreter_2->output(0);

	return ercode;
}

extern "C" float tflitemicro_algo_run_2(float * ptr_image)//int
{
	int ercode = 0;
	//printf("model check1\n");
	for (int i = 0; i < kImageSize_2; i++) 
	{
  		input_2->data.f[i] = *(ptr_image + i);
	}

	//printf("model check2\n");
	TfLiteStatus invoke_status = interpreter_2->Invoke();
	//printf("model check3\n");
	if(invoke_status != kTfLiteOk)
	{
		error_reporter_2->Report("invoke fail\n");
		printf("invoke fail\n");
		//return -1;
	}
	
	//int8_t* results_ptr = output->data.int8;
	//float* results_ptr = output->data.f;
	//printf("model check1\n");
	TfLiteTensor* output = interpreter_2->output(0);
	//printf("model check2\n");
	float* results_ptr = output->data.f;
	//printf("model check3\n");
	float result = results_ptr[0];
	//printf("model check4\n");
    //int result = std::distance(results_ptr, std::max_element(results_ptr, results_ptr + 26));
	//float result = interpreter->output(0)->data.f[0];
	//float probability = std::round(result * 100.0);

	//printf("model check3\n");
	//ercode = result;

	return result;//return ercode
}

