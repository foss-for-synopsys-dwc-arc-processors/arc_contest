/*
 * tflitemicro_algo.cc
 *
 *  Created on: 2020 ~5  27
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

constexpr int tensor_arena_size = 480 * 1024; // 132*1024
#ifdef __GNU__
uint8_t tensor_arena[tensor_arena_size] __attribute__((aligned(16)));
#else
#pragma Bss(".tensor_arena")
uint8_t tensor_arena[tensor_arena_size];
#pragma Bss()
#endif

namespace
{
	tflite::MicroErrorReporter micro_error_reporter;
	tflite::ErrorReporter *error_reporter = nullptr;
	const tflite::Model *model = nullptr;
	tflite::MicroInterpreter *interpreter = nullptr;
	TfLiteTensor *input1 = nullptr;
	TfLiteTensor *input2 = nullptr;
	TfLiteTensor *output = nullptr;
}

// TFu debug log, make it hardware platform print
extern "C" void DebugLog(const char *s) { xprintf("%s", s); } //{ fprintf(stderr, "%s", s); }

extern "C" int tflitemicro_algo_init()
{
	int ercode = 0;
	TfLiteStatus ret;

	error_reporter = &micro_error_reporter;

	error_reporter->Report("TFLM model setting\n");
	model = ::tflite::GetModel(siamesemodel_jv10_lite);
	if (model->version() != TFLITE_SCHEMA_VERSION)
	{
		error_reporter->Report(
			"Model provided is schema version %d not equal "
			"to supported version %d.\n",
			model->version(), TFLITE_SCHEMA_VERSION);
	}

	static tflite::MicroMutableOpResolver<10> micro_op_resolver;
	micro_op_resolver.AddConv2D();
	micro_op_resolver.AddRelu();
	micro_op_resolver.AddMaxPool2D();
	micro_op_resolver.AddReshape();
	micro_op_resolver.AddFullyConnected();
	micro_op_resolver.AddSub();
	micro_op_resolver.AddAbs();
	micro_op_resolver.AddMul();
	micro_op_resolver.AddAdd();
	micro_op_resolver.AddLogistic();
	// micro_op_resolver.AddQuantize();
	//  micro_op_resolver.AddAveragePool2D();

	// Build an interpreter to run the model with.
	static tflite::MicroInterpreter static_interpreter(model, micro_op_resolver,
													   tensor_arena, tensor_arena_size,
													   error_reporter);
	interpreter = &static_interpreter;
	if (interpreter->AllocateTensors() != kTfLiteOk)
	{
		ercode = -1;
	}

	input1 = interpreter->input(0);
	input2 = interpreter->input(1);
	output = interpreter->output(0);

	int aaaa;
	aaaa = interpreter->inputs_size();
	// error_reporter->Report("Model Length %d\n", aaaa);
	return ercode;
}

extern "C" int tflitemicro_algo_run(uint8_t *ptr_image)
{
	int ercode = 0;
	for (int i = 0; i < kImageSize; i++)
	{
		input1->data.int8[i] = *(ptr_image + i);
		/*error_reporter->Report("%d\n", input1->data.int8[i]);
		board_delay_ms(10);*/
	}
	for (int i = 0; i < kImageSize; i++)
	{
		input2->data.int8[i] = *(ptr_image + i + kImageSize);
		/*error_reporter->Report("%d\n", input2->data.int8[i]);
		board_delay_ms(10);*/
	}
	TfLiteStatus invoke_status = interpreter->Invoke();

	if (invoke_status != kTfLiteOk)
	{
		error_reporter->Report("invoke fail\n");
	}

	int8_t *results_ptr = output->data.int8;

	// error_reporter->Report("%s: %d | %s: %d\n", kCategoryLabels[0], *(results_ptr), kCategoryLabels[1], *(results_ptr + 1));

	ercode = *(results_ptr);

	return ercode;
}
