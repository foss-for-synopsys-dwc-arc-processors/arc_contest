#ifndef MODEL_SETTINGS_H_
#define MODEL_SETTINGS_H_

#define kNumCols        1
#define kNumRows        320
#define kImageSize      (kNumCols*kNumRows)
#define kCategoryCount  3

enum {
    M_CATEGORY_SILENCE = 0,
    M_CATEGORY_UNKNOWN,
    M_CATEGORY_HAPPY
/};

extern const char* kCategoryLabels[kCategoryCount];

#endif /* MODEL_SETTINGS_H_ */
