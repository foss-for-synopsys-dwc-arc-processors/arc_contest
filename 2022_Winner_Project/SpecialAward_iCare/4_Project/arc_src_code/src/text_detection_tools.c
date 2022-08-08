#include "text_detection_tools.h"
#include "tflitemicro_algo.h"
/*
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "embARC.h"
#include "embARC_debug.h"
#include "board_config.h"
#include "arc_timer.h"
#include "hx_drv_spi_s.h"
#include "spi_slave_protocol.h"
#include "hardware_config.h"

#include "hx_drv_uart.h"
#define uart_buf_size 100*/

uint32_t std(uint8_t *arr, uint8_t n) {
  uint8_t i;
  uint32_t avg = 0, out = 0;
  int32_t diff;
  for (i = 0; i < n; i++) {
    avg += *(arr + i);
  }
  avg = avg * 100 / n;
  for (i = 0; i < n; i++) {
    diff = (*(arr + i)) * 100 - avg;
    out += (diff > 0) ? diff : -1 * diff;
  }
  return out / n;
}

uint32_t row_std(uint8_t *row, uint32_t len, uint8_t n) {
  uint8_t i;
  uint32_t sum = 0;
  for (i = 0; i < len / n; i++) {
    sum += std(&row[i * n], (i * n + n > len) ? len - i * n : n);
  }
  return sum;
}

uint32_t mean(uint32_t *arr, uint32_t len) {
  uint32_t i, sum = 0;
  for (i = 0; i < len; i++) {
    sum += *(arr + i);
  }
  return sum / len;
}

void find_local_minimum(uint32_t *arr, uint32_t *idx, uint32_t len) {
  uint32_t avg = mean(&arr[0], len);
  uint32_t i, last = 0;
  int8_t cnt = 0, j;
  for (i = 1; i < len - 1; i++) {
    if (arr[i] < avg && arr[i - 1] > arr[i] && arr[i] < arr[i + 1]) {
      for (j = cnt - 1; j >= 0; j--) {
        if (idx[j] / 1000 < i - last) {
          idx[j + 1] = idx[j];
        } else
          break;
      }
      idx[j + 1] = (i - last) * 1000 + i;
      cnt = (cnt == 10) ? 10 : cnt + 1;
      last = i;
    }
  }
}

uint32_t nearest_resize(uint8_t *img, uint8_t *new_img, uint32_t w, uint32_t h,
                        uint32_t new_w, uint32_t new_h) {
  uint32_t i, j, sel_x = 0, sel_y = 0;
  for (i = 0; i < new_h; i++) {
    for (j = 0; j < new_w; j++) {
      // return new_h;
      sel_x = (i * h * 10) / new_h;
      // return sel_x;
      if (sel_x % 10 >= 5) {
        sel_x = (sel_x / 10) + 1;
      } else {
        sel_x = sel_x / 10;
      }

      if (sel_x >= h)
        sel_x = h - 1;

      sel_y = (j * w * 10) / new_w;
      if (sel_y % 10 >= 5) {
        sel_y = (sel_y / 10) + 1;
      } else {
        sel_y = sel_y / 10;
      }
      if (sel_y >= w)
        sel_y = w - 1;

      new_img[i * new_w + j] = img[sel_x * w + sel_y];
    }
  }
}

int text_detection(uint8_t *image, uint8_t *output_img, uint32_t *arr_std,
                   uint32_t *idx, int8_t *input_buf, int *test) {
  /*
  char uart_buf[uart_buf_size] = {0};
  DEV_UART *uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);
  sprintf(uart_buf, "test\r\n");
  uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
      board_delay_ms(10);
  */

  uint16_t j;
  for (j = 0; j < 480; j++) {
    arr_std[j] = row_std(&image[j * 640], 640, 3);
  }

  find_local_minimum(&arr_std[0], &idx[0], 480);

  // tflitemicro_algo_init();

  int i;
  uint32_t k;
  for (i = 0; i < 10; i++) {
    uint32_t h = idx[i] / 1000;
    uint32_t row = (idx[i] % 1000) - h - 1;

    nearest_resize(&image[row * 640], &output_img[0], 640, h, 640, 32);
    for (k = 0; k < 32 * 640; k++) {
      input_buf[k] = output_img[k] - 128;
    }
    test[i] = tflitemicro_algo_run(&input_buf[0]);
  }
  int test_result = 0, cnt = 0;
  for (i = 0; i < 4; i++) {
    if (test[i] != 0) {
      test_result = test[i];
      cnt++;
    }
  }
  if (cnt == 1)
    return test_result;
  else
    return 0;
}