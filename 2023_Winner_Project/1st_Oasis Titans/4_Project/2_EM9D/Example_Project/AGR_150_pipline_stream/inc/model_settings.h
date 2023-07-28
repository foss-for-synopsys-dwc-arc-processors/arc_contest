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

#define kNumCols        150 //96 //224
#define kNumRows        150 //96 //224
#define kNumChannels    1
#define kImageSize      (kNumCols * kNumRows * kNumChannels)

#define kCategoryCount  72

extern const char* kCategoryLabels[kCategoryCount];



#define kNumCols1        150 //96 //224
#define kNumRows1        150 //96 //224
#define kNumChannels1    1
#define kImageSize1      (kNumCols1 * kNumRows1 * kNumChannels1)

#define kCategoryCount1  10

extern const char* kCategoryLabels1[kCategoryCount1];


#endif // MODEL_SETTINGS_H_
