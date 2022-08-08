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

#include "synopsys_sdk_camera_drv.h"

#include "hx_drv_uart.h"
#define uart_buf_size 100


DEV_UART * uart0_ptr;
//DEV_UART * uart2_ptr;

char uart_buf[uart_buf_size] = {0};
char uart_person[uart_buf_size] = {0}; //new

extern uint32_t g_wdma2_baseaddr;


int8_t test_img [kNumRows * kNumCols] = {0};
uint8_t output_img [kNumRows * kNumCols] = {0};
uint8_t output_height = kNumRows;
uint8_t output_width = kNumCols;

int main(void)
{
   
	printf("This is face recognition for shop master\r\n");

    uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);
    uart0_ptr->uart_open(UART_BAUDRATE_115200);

    //uart2_ptr = hx_drv_uart_get_dev(USE_SS_UART_2);
    //uart2_ptr->uart_open(UART_BAUDRATE_57600);

    synopsys_camera_init();

    tflitemicro_algo_init();

    sprintf(uart_person, "0123\r\n");

    int flag[5]={0};

        while(1)
        {
            synopsys_camera_start_capture();
            board_delay_ms(100);

            uint8_t * img_ptr;
            uint32_t img_width = 640;
            uint32_t img_height = 480;
            img_ptr = (uint8_t *) g_wdma2_baseaddr;

            synopsys_camera_down_scaling(img_ptr, img_width, img_height, &output_img[0], output_width, output_height);
     
            for(uint32_t pixel_index = 0; pixel_index < kImageSize; pixel_index ++)
                test_img[pixel_index] = output_img[pixel_index] - 128;

            int32_t test_result = tflitemicro_algo_run(&test_img[0]);
            if(test_result >= 0 && test_result <=4){
                sprintf(uart_buf, "%s\n ", kCategoryLabels[test_result]);
                uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            }
            
            board_delay_ms(10);
            board_delay_ms(2500);
        }     

	return 0;

}

