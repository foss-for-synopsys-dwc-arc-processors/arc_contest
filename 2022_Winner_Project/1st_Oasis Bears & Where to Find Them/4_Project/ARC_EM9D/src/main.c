/**************************************************************************************************
    (C) COPYRIGHT, Himax Technologies, Inc. ALL RIGHTS RESERVED
    ------------------------------------------------------------------------
    File        : main.c
    Project     : WEI
    DATE        : 2018/10/01
    AUTHOR      : 902452
    BRIFE       : main function
    HISTORY     : Initial version - 2018/10/01 created by Will
                : V1.0			  - 2018/11/13 support CLI
**************************************************************************************************/
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

#include "tflitemicro_algo.h"
#include "model_settings.h"
#include "test_samples.h"

#include "synopsys_sdk_camera_drv.h"

#include "hx_drv_uart.h"
#define uart_buf_size 100

DEV_UART *uart0_ptr;
char uart_buf[uart_buf_size] = {0};

extern uint32_t g_wdma2_baseaddr;

int8_t test_img[kNumRows * kNumCols * 2] = {0};
uint8_t output_height = kNumRows;
uint8_t output_width = 66;
// uint8_t output_width = kNumCols;
uint8_t output_img[kNumRows * 133] = {0};

int main(void)
{
    // UART 0 is already initialized with 115200bps
    // printf("This is 2022 ARC Face Recognition Project\r\n");

    uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);

    synopsys_camera_init();

    tflitemicro_algo_init();

    // sprintf(uart_buf, "Start While Loop\r\n");
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
    board_delay_ms(100);
    int counter = 0;
    while (1) // counter<1
    {

        counter++;
        synopsys_camera_start_capture();
        board_delay_ms(1000);

        uint8_t *img_ptr;
        uint32_t img_width = 640;
        uint32_t img_height = 480;
        img_ptr = (uint8_t *)g_wdma2_baseaddr;

        synopsys_camera_down_scaling(img_ptr, img_width, img_height, &output_img[0], output_width, output_height);

        board_delay_ms(1000);

        for (int h = 0; h < kNumRows; h++)
            for (int w = 0; w < kNumCols; w++)
            {
                test_img[w + h * kNumCols] = output_img[8 + w + h * output_width];
            }

        int test_num = 0;
        int test_i = 0;
        int correct_num = 0;

        int test_sum = 0;

        /*
                for (test_num = 0; test_num < 1; test_num++)
                {
                    for (test_i = 0; test_i < kImageSize; test_i++)
                        test_img[test_i + kImageSize] = test_samples[test_num].image[test_i];
                    for (uint32_t heigth_cnt = 0; heigth_cnt < 2 * kNumRows; heigth_cnt++)
                    {
                        for (uint32_t width_cnt = 0; width_cnt < kNumCols; width_cnt++)
                        {
                            if (width_cnt != (kNumCols - 1))
                                sprintf(uart_buf, "%3d, ", test_img[(heigth_cnt * kNumCols) + width_cnt]);
                            else
                                sprintf(uart_buf, "%3d\r\n", test_img[(heigth_cnt * kNumCols) + width_cnt]);

                            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                            board_delay_ms(1);
                        }
                    }
                    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                    board_delay_ms(10);
                    // int32_t test_result = tflitemicro_algo_run(&test_img[0]);

                    // sprintf(uart_buf, "test result %d: %4d \r\n", test_num, test_result);
                    // uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                    // board_delay_ms(10);

                    // test_sum = test_sum + test_result;
                }
                // break;
        */
        int8_t detection = 0;
        int32_t verification = 0;
        int32_t detection_threshold = 0;
        int32_t verification_threshold = 8;
        int8_t verified = 0;

        /*for (test_i = 0; test_i < kImageSize; test_i++)
        {
            test_img[test_i] = test_samples[5].image[test_i] - 128;
        }*/

        for (test_num = 0; test_num < 10; test_num++)
        {
            for (test_i = 0; test_i < kImageSize; test_i++)
            {
                test_img[test_i + kImageSize] = test_samples[test_num].image[test_i] - 128;
            }

            int8_t test_result = tflitemicro_algo_run(&test_img[0]);

            if (test_num != 0)
            {
                test_sum = test_result + test_sum;
                if (test_result > detection_threshold)
                {
                    detection++;
                }

                /*sprintf(uart_buf, "test result %2d: %4d \r\n", test_num, test_result);
                uart0_ptr->uart_write(uart_buf, strlen(uart_buf));*/
            }
            board_delay_ms(10);
        }

        /*int ll = 0;
        int vv = 0;

        for (int nn = 0; nn < 30; nn++)
        {
            for (test_i = 0; test_i < kImageSize; test_i++)
            {
                test_img[test_i] = test_ssamples[nn].image[test_i] - 128;
            }
            ll = 0;
            for (int bb = 0; bb < 50; bb++)
            {
                for (test_i = 0; test_i < kImageSize; test_i++)
                {
                    test_img[test_i + kImageSize] = test_samples[bb].image[test_i] - 128;
                }
                int8_t test_result = tflitemicro_algo_run(&test_img[0]);
                if (test_result > 0)
                {
                    ll = ll + 1;
                }
            }
            if (ll > 40)
            {
                vv = vv + 1;
            }
        }
        */
        verification = detection;

        if (verification > verification_threshold)
        {
            verified = 1;
        }
        else
        {
            verified = 0;
        }
        /*sprintf(uart_buf, "verification: %d \r\n", verification);
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(10);*/
        /*
        sprintf(uart_buf, "verified: %d \r\n", verified);
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(10);
        */
        /*sprintf(uart_buf, "score: %d \r\n", test_sum / 10);
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(10);*/

        /*
        if(counter%5 == 0)
            verified = 1;
        else
            verified = 0;
        */

        sprintf(uart_buf, "%d \r\n", verified);
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(10);

        /*
        for (test_num = 0; test_num < 10; test_num++)
                {
                    for (test_i = 0; test_i < kImageSize; test_i++)
                        test_img[test_i + kImageSize] = test_samples[test_num].image[test_i];
                    int32_t test_result = tflitemicro_algo_run(&test_img[0]);

                    sprintf(uart_buf, "test result %d: %4d \r\n", test_num, test_result);
                    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                    board_delay_ms(10);

                    test_sum = test_sum + test_result;

                    if (test_result > 0)
                    {
                        correct_num++;
                    }
                }

                sprintf(uart_buf, "test sum: %4d \r\n", test_sum);
                uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);

                int score = correct_num * 100 / 10;
                sprintf(uart_buf, "Score: %4d percent \r\n", score);
                uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);

                if (correct_num >= 9)
                {
                    sprintf(uart_buf, "VERIFIED \r\n");
                    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                    board_delay_ms(10);
                }
                else
                {
                    sprintf(uart_buf, "UNVERIFIED \r\n");
                    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                    board_delay_ms(10);
                }

                // board_delay_ms(1000);

        */
    }

    return 0;
}