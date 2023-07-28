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


constexpr int tensor_arena_size = 125*1024;
#ifdef __GNU__
	uint8_t tensor_arena[tensor_arena_size] __attribute__((aligned(16)));
#else
	#pragma Bss(".tensor_arena")
	uint8_t tensor_arena[tensor_arena_size];
	#pragma Bss()
#endif



namespace {
tflite::MicroErrorReporter micro_error_reporter;
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
}

//TFu debug log, make it hardware platform print
extern "C" void DebugLog(const char* s) {xprintf("%s", s);}//{ fprintf(stderr, "%s", s); }

extern "C" int tflitemicro_algo_init()
{
	int ercode = 0;
	TfLiteStatus ret;

	error_reporter = &micro_error_reporter;

	error_reporter->Report("TFLM model setting\n");
	model = ::tflite::GetModel(model_int8_tflite);
	if (model->version() != TFLITE_SCHEMA_VERSION) {
		error_reporter->Report(
			"Model provided is schema version %d not equal "
			"to supported version %d.\n",
			model->version(), TFLITE_SCHEMA_VERSION);
	}


	static tflite::MicroMutableOpResolver<5> micro_op_resolver;
	//micro_op_resolver.AddConv2D();
	//micro_op_resolver.AddMaxPool2D();
	micro_op_resolver.AddLogistic();
	micro_op_resolver.AddFullyConnected();
	micro_op_resolver.AddReshape();
	micro_op_resolver.AddSoftmax();
	micro_op_resolver.AddRelu();


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

/*
extern "C" int* tflitemicro_algo_run(uint8_t * ptr_image)
{
	//int8_t ercode[2] = {0};
	int* ercode =  new int[2];

    for (int i = 0; i < kImageSize; i++) 
      input->data.int8[i] = *(ptr_image + i);

	TfLiteStatus invoke_status = interpreter->Invoke();

	if(invoke_status != kTfLiteOk)
	{
		error_reporter->Report("invoke fail\n");
		//return -1;
	}
	
	int8_t* results_ptr = output->data.int8;
    //int result = std::distance(results_ptr, std::max_element(results_ptr, results_ptr + 26));

	//ercode[0] = static_cast<int>(results_ptr[0]);
	//ercode[1] = static_cast<int>(results_ptr[1]);
	ercode[0] = results_ptr[0];
	ercode[1] = results_ptr[1];

	return ercode;
}*/
extern "C" result_t tflitemicro_algo_run(uint8_t * ptr_image){
	//int ercode =  0;

    for (int i = 0; i < kImageSize; i++)input->data.int8[i] = *(ptr_image + i);

	TfLiteStatus invoke_status = interpreter->Invoke();

	if(invoke_status != kTfLiteOk)	{
		error_reporter->Report("invoke fail\n");
	}
	
	int8_t* results_ptr = output->data.int8;
	//std::cout << "指標的值：" << static_cast<int>(*results_ptr) << std::endl;

	//error_reporter->Report("%s: %d | %s: %d\n", kCategoryLabels[0], *(results_ptr), kCategoryLabels[1],*(results_ptr + 1));
    //int result = std::distance(results_ptr, std::max_element(results_ptr, results_ptr + 26));
	//ercode[0] = static_cast<int>(results_ptr[0]);
	//ercode[1] = static_cast<int>(results_ptr[1]);
	//ercode[0] = results_ptr[0];
	//ercode[1] = results_ptr[1];
	
	result_t result;
	result.BP0 = *(results_ptr + 0);
	result.BP1 = *(results_ptr + 1);

	return result;
}

