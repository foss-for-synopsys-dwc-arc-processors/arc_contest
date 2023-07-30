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
// Testing different models on ARC EM9D develop kit
#include "model.h"



constexpr int tensor_arena_size = 125*1024;
constexpr int tensor_arena_size_2 = 125*1024;
#ifdef __GNU__
	uint8_t tensor_arena[tensor_arena_size] __attribute__((aligned(16)));
	uint8_t tensor_arena_2[tensor_arena_size] __attribute__((aligned(16)));
#else
	#pragma Bss(".tensor_arena")
	uint8_t tensor_arena[tensor_arena_size];
	#pragma Bss()
	#pragma Bss(".tensor_arena_2")
	uint8_t tensor_arena_2[tensor_arena_size_2];
	#pragma Bss()
#endif



namespace {
tflite::MicroErrorReporter micro_error_reporter;
tflite::MicroErrorReporter micro_error_reporter_2;
tflite::ErrorReporter* error_reporter = nullptr;
tflite::ErrorReporter* error_reporter_2 = nullptr;
const tflite::Model* model = nullptr;
const tflite::Model* model_2 = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
tflite::MicroInterpreter* interpreter_2 = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* input_2 = nullptr;
TfLiteTensor* output = nullptr;
TfLiteTensor* output_2 = nullptr;
}

//TFu debug log, make it hardware platform print
extern "C" void DebugLog(const char* s) {xprintf("%s", s);}//{ fprintf(stderr, "%s", s); }

extern "C" int tflitemicro_algo_init()
{
	int ercode = 0;
	TfLiteStatus ret;

	error_reporter = &micro_error_reporter;
	error_reporter_2 = &micro_error_reporter_2;

	error_reporter->Report("TFLM model setting\n");
	error_reporter_2->Report("TFLM model setting\n");
	model = ::tflite::GetModel(person_detect_model_int8_tflite);
	model_2 = ::tflite::GetModel(conv_dense5_i2_tflite);
	if (model->version() != TFLITE_SCHEMA_VERSION) {
		error_reporter->Report(
			"Model provided is schema version %d not equal "
			"to supported version %d.\n",
			model->version(), TFLITE_SCHEMA_VERSION);
	}

	if (model_2->version() != TFLITE_SCHEMA_VERSION) {
		error_reporter_2->Report(
			"model_2 provided is schema version %d not equal "
			"to supported version %d.\n",
			model_2->version(), TFLITE_SCHEMA_VERSION);
	}




	static tflite::MicroMutableOpResolver<5> micro_op_resolver;
	micro_op_resolver.AddAveragePool2D();
	micro_op_resolver.AddConv2D();
	micro_op_resolver.AddDepthwiseConv2D();
	micro_op_resolver.AddReshape();
	micro_op_resolver.AddSoftmax();

	static tflite::MicroMutableOpResolver<4> micro_op_resolver_2;
	micro_op_resolver_2.AddConv2D();
	micro_op_resolver_2.AddMaxPool2D();
	micro_op_resolver_2.AddFullyConnected();
	micro_op_resolver_2.AddReshape();
	


	


	// Build an interpreter to run the model with.
	static tflite::MicroInterpreter static_interpreter(model, micro_op_resolver,
										   tensor_arena, tensor_arena_size,
										   error_reporter);

	static tflite::MicroInterpreter static_interpreter_2(model_2, micro_op_resolver_2,
										   tensor_arena_2, tensor_arena_size_2,
										   error_reporter_2);
										   
	interpreter = &static_interpreter;
	interpreter_2 = &static_interpreter_2;
	if(interpreter->AllocateTensors()!= kTfLiteOk) {
		ercode = -1;
	}

	if(interpreter_2->AllocateTensors()!= kTfLiteOk) {
		ercode = -1;
	}

	input = interpreter->input(0);
	output = interpreter->output(0);

	input_2 = interpreter_2->input(0);
	output_2 = interpreter_2->output(0);
	

	return ercode;
}

extern "C" int tflitemicro_algo_run(uint8_t * ptr_image)
{
	int ercode = 0;

    for (int i = 0; i < kImageSize; i++) 
      input->data.int8[i] = *(ptr_image + i);

	
	TfLiteStatus invoke_status = interpreter->Invoke();

	if(invoke_status != kTfLiteOk)
	{
		error_reporter->Report("invoke fail\n");
		return -1;
	}
	
	int8_t* results_ptr = output->data.int8;
	
	error_reporter->Report("%s: %d\n", kCategoryLabels[1], *(results_ptr + 1));

	ercode = *(results_ptr + 1) ;

	return ercode;
}


extern "C" int tflitemicro_algo_run_d(uint8_t * danger_param)
{
	int ercode = 0;

    for (int i = 0; i < kImageSize_d; i++) 
      input_2->data.int8[i] = *(danger_param + i);

	
	TfLiteStatus invoke_status = interpreter_2->Invoke();

	if(invoke_status != kTfLiteOk)
	{
		error_reporter->Report("invoke2 fail\n");
		return -1;
	}
	
	
	int8_t* results_ptr = output_2->data.int8;

	error_reporter->Report("%s: %d\n", kCategoryLabels_d[0], *(results_ptr));

	ercode = *(results_ptr + 1) ;
	
	return ercode;
}
