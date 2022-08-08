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

struct TestSample
{
  uint8_t image[kImageSize];
};

extern const int kNumSamples;
extern const struct TestSample test_samples[];