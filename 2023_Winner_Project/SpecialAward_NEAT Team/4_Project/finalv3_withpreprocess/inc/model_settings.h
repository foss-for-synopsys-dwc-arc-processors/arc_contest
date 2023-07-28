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

// for Rssi
#define kNumDatas    60
#define kNumChannels    1
#define kNumDatasize    kNumDatas*kNumChannels
#define kCategoryCount  3
extern const int kCategoryLabels[kCategoryCount];

//for Acc
#define kNumDatas_acc    60
#define kNumChannels_acc    1
#define kNumDatasize_acc    kNumDatas_acc*kNumChannels_acc
#define kCategoryCount_acc  2
extern const int kCategoryLabels_acc[kCategoryCount_acc];


#endif // MODEL_SETTINGS_H_
