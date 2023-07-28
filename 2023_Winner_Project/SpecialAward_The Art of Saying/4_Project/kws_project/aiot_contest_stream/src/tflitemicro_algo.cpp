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
#include "model.h"
#include <cstdio>


constexpr int tensor_arena_size = 300*1024;
#ifdef __GNU__
	uint8_t tensor_arena[tensor_arena_size] __attribute__((aligned(16)));
#else
	#pragma Bss(".tensor_arena")
	uint8_t tensor_arena[tensor_arena_size];
	#pragma Bss()
#endif

struct model_extern_stat {
    TfLiteTensor* input;
    TfLiteTensor* output;
    size_t in_dim_size;
    size_t out_dim_size;
};

namespace {
tflite::MicroErrorReporter micro_error_reporter;
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;

// stream svdf xxxs
#define MAX_EXTERNAL_STAT   4
#define MAX_STAT            (MAX_EXTERNAL_STAT + 1)
static struct model_extern_stat mes[MAX_STAT];
}

extern "C" int tflitemicro_algo_init()
{
	int ercode = 0;
	TfLiteStatus ret;

	error_reporter = &micro_error_reporter;

	model = ::tflite::GetModel(svdf_happy_e_tflite);
	if (model->version() != TFLITE_SCHEMA_VERSION) {
		error_reporter->Report(
			"Model provided is schema version %d not equal "
			"to supported version %d.\n",
			model->version(), TFLITE_SCHEMA_VERSION);
	}


	static tflite::MicroMutableOpResolver<10> micro_op_resolver;
	micro_op_resolver.AddAudio_Spectrogram();
	micro_op_resolver.AddMFCC();
	micro_op_resolver.AddFullyConnected();
	micro_op_resolver.AddReshape();
	micro_op_resolver.AddDepthwiseConv2D();
	micro_op_resolver.AddAdd();
	micro_op_resolver.AddSum();
	micro_op_resolver.AddStridedSlice();
	micro_op_resolver.AddConcatenation();
	micro_op_resolver.AddMul();


	static tflite::MicroInterpreter static_interpreter(model, micro_op_resolver,
										   tensor_arena, tensor_arena_size,
										   error_reporter);
	interpreter = &static_interpreter;
	if(interpreter->AllocateTensors()!= kTfLiteOk) {
		ercode = -1;
	}

	for (int i = 0; i < MAX_STAT; ++i) {
		mes[i].input  = interpreter->input(i);
		mes[i].output = interpreter->output(i);

		mes[i].in_dim_size = 1;
		mes[i].out_dim_size = 1;
		for (int j = 0; j < mes[i].input->dims->size; ++j) {
			mes[i].in_dim_size *= mes[i].input->dims->data[j];
			mes[i].out_dim_size *= mes[i].output->dims->data[j];
		}
	}

	return ercode;
}

extern "C" int tflitemicro_algo_run(float * ptr_image)
{
	const int mes_cp_seq[MAX_EXTERNAL_STAT] = {4, 1, 2, 3};
	int i, j;

	for (j = 0; j < MAX_EXTERNAL_STAT; j++)
		memcpy(mes[mes_cp_seq[j]].input->data.f, mes[mes_cp_seq[j]].output->data.f, sizeof(float)*mes[mes_cp_seq[j]].in_dim_size);
	memcpy(mes[0].input->data.f, ptr_image + i*mes[0].in_dim_size, sizeof(float)*mes[0].in_dim_size);

	TfLiteStatus invoke_status = interpreter->Invoke();

	if(invoke_status != kTfLiteOk)
		error_reporter->Report("invoke fail\n");

	float *results_ptr = mes[0].output->data.f;
	results_ptr[0] += 140.0;
	int result = std::distance(results_ptr, std::max_element(results_ptr, results_ptr + mes[0].out_dim_size));

	return result;
}
