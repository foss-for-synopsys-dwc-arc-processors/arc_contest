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
#include <library/cv/tflitemicro_24/tensorflow/lite/schema/schema_generated.h>
#include <library/cv/tflitemicro_24/tensorflow/lite/version.h>

#include "tflitemicro_algo.h"
#include "model_settings.h"
#include "model_c.h"
#include "model_i.h"

constexpr int tensor_arena_size = 370*1024;
#ifdef __GNU__
	uint8_t tensor_arena_c[tensor_arena_size] __attribute__((aligned(16)));
	uint8_t tensor_arena_i[tensor_arena_size] __attribute__((aligned(16)));
#else
	#pragma Bss(".tensor_arena")
	uint8_t tensor_arena_c[tensor_arena_size];
	uint8_t tensor_arena_i[tensor_arena_size];
	#pragma Bss()
#endif



namespace {
	tflite::MicroErrorReporter micro_error_reporter_c;
	tflite::MicroErrorReporter micro_error_reporter_i;
	tflite::ErrorReporter* error_reporter_c = nullptr;
	tflite::ErrorReporter* error_reporter_i = nullptr;
	const tflite::Model* model_c = nullptr;
	const tflite::Model* model_i = nullptr;
	tflite::MicroInterpreter* interpreter_c = nullptr;
	tflite::MicroInterpreter* interpreter_i = nullptr;
	TfLiteTensor* input_c = nullptr;
	TfLiteTensor* input_i = nullptr;
	TfLiteTensor* output_c = nullptr;
	TfLiteTensor* output_i = nullptr;
}

//TFu debug log, make it hardware platform print
extern "C" void DebugLog(const char* s) {xprintf("%s", s);}//{ fprintf(stderr, "%s", s); }

extern "C" int tflitemicro_algo_init()
{
	int ercode_c = 0;
	int ercode_i = 0;
	TfLiteStatus ret;

	error_reporter_c = &micro_error_reporter_c;
	error_reporter_i = &micro_error_reporter_i;

	error_reporter_c->Report("TFLM model setting\n");
	error_reporter_i->Report("TFLM model setting\n");
	
	model_c = ::tflite::GetModel(tflite_model_quant_int8_c_tflite);
	if (model_c->version() != TFLITE_SCHEMA_VERSION) {
		error_reporter_c->Report(
			"Model provided is schema version %d not equal "
			"to supported version %d.\n",
			model_c->version(), TFLITE_SCHEMA_VERSION);
	}


	static tflite::MicroMutableOpResolver<10> micro_op_resolver_c;
	micro_op_resolver_c.AddConv2D();
	micro_op_resolver_c.AddMaxPool2D();
	micro_op_resolver_c.AddFullyConnected();
	micro_op_resolver_c.AddReshape();
	micro_op_resolver_c.AddSoftmax();
	micro_op_resolver_c.AddRelu();
	micro_op_resolver_c.AddMul();
	micro_op_resolver_c.AddAdd();
	micro_op_resolver_c.AddQuantize();
	micro_op_resolver_c.AddDequantize();


	// Build an interpreter to run the model with.
	static tflite::MicroInterpreter static_interpreter_c(model_c, micro_op_resolver_c,
										   tensor_arena_c, tensor_arena_size,
										   error_reporter_c);
	interpreter_c = &static_interpreter_c;
	if(interpreter_c->AllocateTensors()!= kTfLiteOk) {
		ercode_c = -1;
	}

	input_c = interpreter_c->input(0);
	output_c = interpreter_c->output(0);

//--------------------------------------------------------------------------------------------------//
	model_i = ::tflite::GetModel(tflite_model_quant_int8_i_tflite);
	if (model_i->version() != TFLITE_SCHEMA_VERSION) {
		error_reporter_i->Report(
			"Model provided is schema version %d not equal "
			"to supported version %d.\n",
			model_i->version(), TFLITE_SCHEMA_VERSION);
	}


	static tflite::MicroMutableOpResolver<11> micro_op_resolver_i;
	micro_op_resolver_i.AddConv2D();
	micro_op_resolver_i.AddMaxPool2D();
	micro_op_resolver_i.AddAveragePool2D();
	micro_op_resolver_i.AddFullyConnected();
	micro_op_resolver_i.AddReshape();
	micro_op_resolver_i.AddSoftmax();
	micro_op_resolver_i.AddRelu();
	micro_op_resolver_i.AddMul();
	micro_op_resolver_i.AddAdd();
	micro_op_resolver_i.AddQuantize();
	micro_op_resolver_i.AddDequantize();


	// Build an interpreter to run the model with.
	static tflite::MicroInterpreter static_interpreter_i(model_i, micro_op_resolver_i,
										   tensor_arena_i, tensor_arena_size,
										   error_reporter_i);
	interpreter_i = &static_interpreter_i;
	if(interpreter_i->AllocateTensors()!= kTfLiteOk) {
		ercode_i = -1;
	}

	input_i = interpreter_i->input(0);
	output_i = interpreter_i->output(0);

	return ercode_i;
}


extern "C" ans tflitemicro_algo_run_c(int8_t * ptr_image_c, int kc, int8_t * ptr_image_i, int ki)
{
	int ercode_c= 0, ercode_i = 0;
	
	for (int i = 0; i < ki; i++){
      input_i->data.int8[i] = *(ptr_image_i + i);
	}
	for (int i = 0; i < kc; i++){
      input_c->data.int8[i] = *(ptr_image_c + i);
	}
	TfLiteStatus invoke_status_c = interpreter_c->Invoke();

	if(invoke_status_c != kTfLiteOk)
	{
		error_reporter_c->Report("invoke fail\n");
	}

	int8_t* results_ptr_c = output_c->data.int8;


    int result_c = std::distance(results_ptr_c, std::max_element(results_ptr_c, results_ptr_c + 2));
	
	ercode_c = result_c;

	TfLiteStatus invoke_status_i = interpreter_i->Invoke();

	if(invoke_status_i != kTfLiteOk)
	{
		error_reporter_i->Report("invoke fail\n");
	}

	int8_t* results_ptr_i = output_i->data.int8;


    int result_i = std::distance(results_ptr_i, std::max_element(results_ptr_i, results_ptr_i + 4));
	
	ercode_i = result_i;
	ans result = {ercode_c, ercode_i};
	return result;
}


