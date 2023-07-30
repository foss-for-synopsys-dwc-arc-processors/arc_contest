/*
* Copyright 2019-2020, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/
#ifndef MODEL_SETTINGS_H_
#define MODEL_SETTINGS_H_

#define kNumCols        96
#define kNumRows        96
#define kNumChannels    1
#define kImageSize      (kNumCols * kNumRows * kNumChannels)

#define kCategoryCount  2

extern const char* kCategoryLabels[kCategoryCount];


#define kNumCols_d        1
#define kNumRows_d        3
#define kNumChannels_d    1
#define kImageSize_d      (kNumCols_d * kNumRows_d * kNumChannels_d)

#define kCategoryCount_d  1

extern const char* kCategoryLabels_d[kCategoryCount_d];
#endif // MODEL_SETTINGS_H_
