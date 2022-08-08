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
//#include
//<library/cv/tflitemicro_24/tensorflow/lite/micro/testing/micro_test.h>
#include <library/cv/tflitemicro_24/tensorflow/lite/schema/schema_generated.h>
#include <library/cv/tflitemicro_24/tensorflow/lite/version.h>

#include "tflitemicro_algo.h"
#include "model_settings.h"
//#include "model_ver2.h"
//#include "yolo.h"
//#include "model.h"
#include "final_model_ver2.h"

// constexpr int tensor_arena_size = 136*1024;
constexpr int tensor_arena_size = 404 * 1024;
#ifdef __GNU__
uint8_t tensor_arena[tensor_arena_size] __attribute__((aligned(16)));
#else
#pragma Bss(".tensor_arena")
uint8_t tensor_arena[tensor_arena_size];
#pragma Bss()
#endif

namespace {
tflite::MicroErrorReporter micro_error_reporter;
tflite::ErrorReporter *error_reporter = nullptr;
const tflite::Model *model = nullptr;
tflite::MicroInterpreter *interpreter = nullptr;
TfLiteTensor *input = nullptr;
TfLiteTensor *output = nullptr;
}

// TFu debug log, make it hardware platform print
extern "C" void DebugLog(const char *s) {
  xprintf("%s", s);
} //{ fprintf(stderr, "%s", s); }

extern "C" int tflitemicro_algo_init() {
  int ercode = 0;
  TfLiteStatus ret;

  error_reporter = &micro_error_reporter;

  error_reporter->Report("TFLM model setting\n");
  model = ::tflite::GetModel(model_int8_tflite);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report("Model provided is schema version %d not equal "
                           "to supported version %d.\n",
                           model->version(), TFLITE_SCHEMA_VERSION);
  }

  static tflite::MicroMutableOpResolver<6> micro_op_resolver;
  /*
  micro_op_resolver.AddConv2D();
  micro_op_resolver.AddMaxPool2D();
  micro_op_resolver.AddFullyConnected();
  micro_op_resolver.AddReshape();
  micro_op_resolver.AddSoftmax();
  micro_op_resolver.AddRelu();
  */
  //用來看memory使用率
  // micro_op_resolver.AddSplitV();

  // micro_op_resolver.AddAbs();
  // micro_op_resolver.AddAdd();
  // micro_op_resolver.AddArgMax();
  // micro_op_resolver.AddArgMin();
  // micro_op_resolver.AddAveragePool2D();
  // micro_op_resolver.AddCeil();
  // micro_op_resolver.AddCircularBuffer();
  // micro_op_resolver.AddConcatenation();
  micro_op_resolver.AddConv2D();
  // micro_op_resolver.AddCos();
  // micro_op_resolver.AddDepthwiseConv2D();
  // micro_op_resolver.AddDequantize();
  // micro_op_resolver.AddEqual();
  // micro_op_resolver.AddFloor();
  micro_op_resolver.AddFullyConnected();
  // micro_op_resolver.AddGreater();
  // micro_op_resolver.AddGreaterEqual();
  // micro_op_resolver.AddHardSwish();
  // micro_op_resolver.AddL2Normalization();
  // micro_op_resolver.AddLess();
  // micro_op_resolver.AddLessEqual();
  // micro_op_resolver.AddLog();
  // micro_op_resolver.AddLogicalAnd();
  // micro_op_resolver.AddLogicalNot();
  // micro_op_resolver.AddLogicalOr();
  // micro_op_resolver.AddLogistic();
  // micro_op_resolver.AddMaximum();
  micro_op_resolver.AddMaxPool2D();
  // micro_op_resolver.AddMean();
  // micro_op_resolver.AddMinimum();
  // micro_op_resolver.AddMul();
  // micro_op_resolver.AddNeg();
  // micro_op_resolver.AddNotEqual();
  // micro_op_resolver.AddPack();
  // micro_op_resolver.AddPad();
  // micro_op_resolver.AddPadV2();
  // micro_op_resolver.AddPrelu();
  // micro_op_resolver.AddQuantize();
  // micro_op_resolver.AddReduceMax();
  micro_op_resolver.AddRelu();
  // micro_op_resolver.AddRelu6();
  micro_op_resolver.AddReshape();
  // micro_op_resolver.AddResizeNearestNeighbor();
  // micro_op_resolver.AddRound();
  // micro_op_resolver.AddRsqrt();
  // micro_op_resolver.AddSin();
  micro_op_resolver.AddSoftmax();
  // micro_op_resolver.AddSplit();
  // micro_op_resolver.AddSplitV();
  // micro_op_resolver.AddSqrt();
  // micro_op_resolver.AddSquare();
  // micro_op_resolver.AddStridedSlice();
  // micro_op_resolver.AddSub();
  // micro_op_resolver.AddSvdf();
  // micro_op_resolver.AddTanh();
  // micro_op_resolver.AddUnpack();

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, micro_op_resolver, tensor_arena, tensor_arena_size,
      error_reporter);

  interpreter = &static_interpreter;
  if (interpreter->AllocateTensors() != kTfLiteOk) {
    ercode = -1;
  }

  input = interpreter->input(0);
  output = interpreter->output(0);

  // error_reporter->Report("OK2\n");

  return ercode;
}

extern "C" int tflitemicro_algo_run(uint8_t *ptr_image) {
  int ercode = 0;

  for (int i = 0; i < kImageSize; i++)
    input->data.int8[i] = *(ptr_image + i);

  TfLiteStatus invoke_status = interpreter->Invoke();

  if (invoke_status != kTfLiteOk) {
    error_reporter->Report("invoke fail\n");
  }

  int8_t *results_ptr = output->data.int8;
  int result = std::distance(results_ptr,
                             std::max_element(results_ptr, results_ptr + 5));

  ercode = result;

  return ercode;
}
