/*
* Copyright 2019-2020, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

#include "model_settings.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct TestSample_c {
  int label;
  uint8_t image[kImageSize_c];
};
struct TestSample_i {
  int label;
  uint8_t image[kImageSize_i];
};

extern const int kNumSamples;

extern const struct TestSample_c test_samples_c[];
extern const struct TestSample_i test_samples_i[];