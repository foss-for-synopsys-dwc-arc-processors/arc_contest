#ifndef MODEL_SETTINGS_H_
#define MODEL_SETTINGS_H_

#define kNumInputElements 150
#define kNumOutputElements 1

#define kNumInputElements_2 30

#define kCategoryCount 2

#define kImageSize      (kNumInputElements * kNumOutputElements )
#define kImageSize_2      (kNumInputElements_2 * kNumOutputElements )

extern const char* kCategoryLabels[kCategoryCount];

#endif  // MODEL_SETTINGS_H_
