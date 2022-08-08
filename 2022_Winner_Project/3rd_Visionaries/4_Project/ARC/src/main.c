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
#define uart_buf_size 200
#define str_buf_size 4340
#define input_data_size 2170

DEV_UART * uart0_ptr;


#include "tflitemicro_algo.h"
#include "model_settings.h"

#include "test_samples.h"

char uart_buf[uart_buf_size] = {0};
char str_buf[str_buf_size] = {0};
int8_t input_buf_c[kImageSize_c] = {0};
int8_t input_buf_i[kImageSize_i] = {0};
uint8_t input_data[input_data_size] = {0};

int main(void)
{    
    //UART 0 is already initialized with 115200bps
    uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);

    int test_i = 0;
    int test_j = 0;
    int test_correct_c = 0;
    int test_correct_i = 0;
    int test_result_c = 0;
    int test_result_i = 0;

    tflitemicro_algo_init();

    //uart receive
    int c;
    char covid_result;
    char str[2]={0};

    while (1) {
        c = uart0_ptr->uart_read(&str_buf, str_buf_size);
        
		if (c == str_buf_size)
		{
			for(int i=0; i<input_data_size; i++)
            {
                str[0] = str_buf[i*2];
                str[1] = str_buf[i*2+1];

                input_data[i] = (uint8_t)strtol(str, NULL, 16);
            }
            //uart end

            for(test_j = 0; test_j < kImageSize_c; test_j ++) // kImageSize = 370
                input_buf_c[test_j] = input_data[1800+test_j]-128;
            
            for(test_j = 0; test_j < kImageSize_i; test_j ++) // kImageSize = 1800
                input_buf_i[test_j] = input_data[test_j]-128;
    

            struct ans result;
            result = tflitemicro_algo_run_c(&input_buf_c[0], kImageSize_c, &input_buf_i[0], kImageSize_i);


            test_result_c = result.ercode_c;
            test_result_i = result.ercode_i;

            if(test_result_c==0)
                covid_result = 'F';
            else
                covid_result = 'T';


            sprintf(uart_buf, "%d %c", test_result_i, covid_result);
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);
		}
        board_delay_ms(1);
	}

 
	return 0;
}

