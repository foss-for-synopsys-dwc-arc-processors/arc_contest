/*
 * tflitemicro_algo.cc
 *
 *  Created on: 2020�~5��27��
 *      Author: 902447
 */


// the TfLite library in Synopsys sdk
#include <library/cv/tflitemicro_24/tensorflow/lite/c/common.h>
#include <library/cv/tflitemicro_24/tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include <library/cv/tflitemicro_24/tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include <library/cv/tflitemicro_24/tensorflow/lite/micro/kernels/micro_ops.h>
#include <library/cv/tflitemicro_24/tensorflow/lite/micro/micro_error_reporter.h>
#include <library/cv/tflitemicro_24/tensorflow/lite/micro/micro_interpreter.h>
#include <library/cv/tflitemicro_24/tensorflow/lite/schema/schema_generated.h>
#include <library/cv/tflitemicro_24/tensorflow/lite/version.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "arc_timer.h"
#include "board_config.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "embARC_error.h"

#include "model.h"
#include "model_settings.h"
#include "tflitemicro_algo.h"


constexpr int tensor_arena_size = 407680;

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
}  // namespace


// TFu debug log, make it hardware platform print
extern "C" void DebugLog(const char* s) { xprintf("%s", s); }  //{ fprintf(stderr, "%s", s); }

extern "C" int tflitemicro_algo_init() {
    int ercode = 0;
    TfLiteStatus ret;

    error_reporter = &micro_error_reporter;

    error_reporter->Report("TFLM model setting\n");
    model = ::tflite::GetModel(generated_resnet_model_int8_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        error_reporter->Report(
            "Model provided is schema version %d not equal "
            "to supported version %d.\n",
            model->version(), TFLITE_SCHEMA_VERSION);
    }

    static tflite::MicroMutableOpResolver<16> micro_op_resolver;
    // Add the op resolver Resnet need
    micro_op_resolver.AddAveragePool2D();
    micro_op_resolver.AddConv2D();
    micro_op_resolver.AddDepthwiseConv2D();
    micro_op_resolver.AddReshape();
    micro_op_resolver.AddSoftmax();
    micro_op_resolver.AddPad();
    micro_op_resolver.AddLogistic();
    micro_op_resolver.AddMul();
    micro_op_resolver.AddStridedSlice();
    micro_op_resolver.AddAdd();
    micro_op_resolver.AddConcatenation();
    micro_op_resolver.AddMaxPool2D();
    micro_op_resolver.AddFullyConnected();
	micro_op_resolver.AddQuantize();
    micro_op_resolver.AddResizeNearestNeighbor();
    micro_op_resolver.AddRelu();
    

    // Build an interpreter to run the model with.
    static tflite::MicroInterpreter static_interpreter(model, micro_op_resolver, tensor_arena, tensor_arena_size,
                                                       error_reporter); // old one
                                                       
    interpreter = &static_interpreter;
    if (interpreter->AllocateTensors() != kTfLiteOk) {
        ercode = -1;
    }

    input = interpreter->input(0);
    output = interpreter->output(0);

    return ercode;
}

extern "C" int tflitemicro_algo_run(uint8_t* ptr_image) {
    int ercode = 0;

    for (int i = 0; i < kImageSize; i++) input->data.int8[i] = *(ptr_image + i);

    TfLiteStatus invoke_status = interpreter->Invoke();

    if (invoke_status != kTfLiteOk) {
        error_reporter->Report("invoke fail\n");
    }

    int8_t* results_ptr = output->data.int8;

    error_reporter->Report("%s: %d | %s: %d\n", kCategoryLabels[0], *(results_ptr), kCategoryLabels[1],
                           *(results_ptr + 1));

    // find the label with the highest probability 
    for(int i=0; i<kCategoryCount; i++){
        if (results_ptr[i] >= results_ptr[ercode]){
            ercode = i;
        }
    }

    // return the highest label
    return ercode;
}