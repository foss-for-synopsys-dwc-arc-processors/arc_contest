#include <stdio.h>

uint32_t std(uint8_t *arr, uint8_t n);
uint32_t row_std(uint8_t *row, uint32_t len, uint8_t n);
uint32_t mean(uint32_t *arr, uint32_t len);
void find_local_minimum(uint32_t *arr, uint32_t *idx, uint32_t len);
uint32_t nearest_resize(uint8_t *img, uint8_t *new_img,uint32_t w,uint32_t h,uint32_t new_w,uint32_t new_h);
int text_detection(uint8_t *image, uint8_t *output_img, uint32_t *arr_std, uint32_t *idx, int8_t *input_buf, int *test);
//uint32_t STD(uint8_t *arr, uint8_t n);
//uint32_t row_std(uint8_t *row, uint32_t len, uint8_t n);