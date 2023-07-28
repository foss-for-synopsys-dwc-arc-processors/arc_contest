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
#include "model1.h"

//
// DEV_UART * uart0_ptr;
// char uart_buf[uart_buf_size] = {0};
//

constexpr int tensor_arena_size = 240*1024; //125*1024 //250*124 //420*1024 //300
#ifdef __GNU__
	uint8_t tensor_arena[tensor_arena_size] __attribute__((aligned(16)));
#else
	#pragma Bss(".tensor_arena")
	uint8_t tensor_arena[tensor_arena_size];
	#pragma Bss()
#endif

constexpr int tensor_arena_size1 = 240*1024; //125*1024 //250*124 //160*1024 //120*1024
#ifdef __GNU__
	uint8_t tensor_arena1[tensor_arena_size1] __attribute__((aligned(16)));
#else
	#pragma Bss(".tensor_arena")
	uint8_t tensor_arena1[tensor_arena_size1];
	#pragma Bss()
#endif



namespace {
tflite::MicroErrorReporter micro_error_reporter;
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;

tflite::MicroErrorReporter micro_error_reporter1;
tflite::ErrorReporter* error_reporter1 = nullptr;
const tflite::Model* model1 = nullptr;
tflite::MicroInterpreter* interpreter1 = nullptr;
TfLiteTensor* input1 = nullptr;
TfLiteTensor* output1 = nullptr;
}

//TFu debug log, make it hardware platform print
extern "C" void DebugLog(const char* s) {xprintf("%s", s);}//{ fprintf(stderr, "%s", s); }

extern "C" int tflitemicro_algo_init()
{
	int ercode = 0;
	TfLiteStatus ret;

	error_reporter = &micro_error_reporter;

	error_reporter->Report("TFLM model setting\n");
	model = ::tflite::GetModel(person_detect_model_int8_tflite);
	if (model->version() != TFLITE_SCHEMA_VERSION) {
		error_reporter->Report(
			"Model provided is schema version %d not equal "
			"to supported version %d.\n",
			model->version(), TFLITE_SCHEMA_VERSION);
	}


	static tflite::MicroMutableOpResolver<8> micro_op_resolver;
	micro_op_resolver.AddAveragePool2D();
	micro_op_resolver.AddConv2D();
	micro_op_resolver.AddDepthwiseConv2D();
	micro_op_resolver.AddReshape();
	micro_op_resolver.AddSoftmax();

	//
	micro_op_resolver.AddAdd();
	micro_op_resolver.AddMean();
	micro_op_resolver.AddFullyConnected();

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

extern "C" int tflitemicro_algo_run(uint8_t * ptr_image)
{
	int ercode = 0;

    for (int i = 0; i < kImageSize; i++) 
      input->data.int8[i] = *(ptr_image + i);

	TfLiteStatus invoke_status = interpreter->Invoke();

	if(invoke_status != kTfLiteOk)
	{
		//error_reporter->Report("invoke fail\n");
		return -1;
	}
	
	int8_t* results_ptr = output->data.int8;
	
	

	int maxIndex = -1;
  	int maxScore = -255;
	for (int i = 0; i < kCategoryCount; i++) 
	{
		// error_reporter->Report("[%d]: %d", i, results_ptr[i]);
		
		// sprintf(uart_buf, "[%d]: %d \r\n", i, results_ptr[i]);
		// uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
		board_delay_ms(10);


		if ( maxScore < results_ptr[i] )
		{
			maxScore = results_ptr[i];
			maxIndex = i;
		}

		// if ((results_ptr[i] > 0) && (maxScore < results_ptr[i]))
		// {
		// 	maxScore = results_ptr[i];
		// 	maxIndex = i;
		// }
	}

	ercode = maxIndex;
	
	
	// error_reporter->Report("%s: %d | %s: %d\n", kCategoryLabels[0], *(results_ptr), kCategoryLabels[1], *(results_ptr + 1));
	// ercode = *(results_ptr + 1) ;

	return ercode;
}





extern "C" int tflitemicro_algo_init1()
{
	int ercode = 0;
	TfLiteStatus ret;

	error_reporter1 = &micro_error_reporter1;

	error_reporter1->Report("TFLM model1 setting\n");
	model1 = ::tflite::GetModel(detect_model_int8_tflite);
	if (model1->version() != TFLITE_SCHEMA_VERSION) {
		error_reporter1->Report(
			"Model provided is schema version %d not equal "
			"to supported version %d.\n",
			model1->version(), TFLITE_SCHEMA_VERSION);
	}


	static tflite::MicroMutableOpResolver<8> micro_op_resolver;
	micro_op_resolver.AddAveragePool2D();
	micro_op_resolver.AddConv2D();
	micro_op_resolver.AddDepthwiseConv2D();
	micro_op_resolver.AddReshape();
	micro_op_resolver.AddSoftmax();

	//
	micro_op_resolver.AddAdd();
	micro_op_resolver.AddMean();
	micro_op_resolver.AddFullyConnected();

	// Build an interpreter to run the model with.
	static tflite::MicroInterpreter static_interpreter(model1, micro_op_resolver,
										   tensor_arena1, tensor_arena_size1,
										   error_reporter1);
	interpreter1 = &static_interpreter;
	if(interpreter1->AllocateTensors()!= kTfLiteOk) {
		ercode = -1;
	}

	input1 = interpreter1->input(0);
	output1 = interpreter1->output(0);



	return ercode;
}

extern "C" int tflitemicro_algo_run1(uint8_t * ptr_image)
{
	int ercode = 0;

    for (int i = 0; i < kImageSize1; i++) 
      input1->data.int8[i] = *(ptr_image + i);

	TfLiteStatus invoke_status = interpreter1->Invoke();

	if(invoke_status != kTfLiteOk)
	{
		//error_reporter->Report("invoke fail\n");
		return -1;
	}

	int8_t* results_ptr = output1->data.int8;
	
	
	int maxIndex = -1;
  	int maxScore = -255;
	for (int i = 0; i < kCategoryCount1; i++) 
	{
		// error_reporter->Report("[%d]: %d", i, results_ptr[i]);
		
		// sprintf(uart_buf, "[%d]: %d \r\n", i, results_ptr[i]);
		// uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
		board_delay_ms(10);


		if ( maxScore < results_ptr[i] )
		{
			maxScore = results_ptr[i];
			maxIndex = i;
		}

		// if ((results_ptr[i] > 0) && (maxScore < results_ptr[i]))
		// {
		// 	maxScore = results_ptr[i];
		// 	maxIndex = i;
		// }
	}
	
	ercode = maxIndex;
	
	
	// error_reporter->Report("%s: %d | %s: %d\n", kCategoryLabels[0], *(results_ptr), kCategoryLabels[1], *(results_ptr + 1));
	// ercode = *(results_ptr + 1) ;

	return ercode;

}

