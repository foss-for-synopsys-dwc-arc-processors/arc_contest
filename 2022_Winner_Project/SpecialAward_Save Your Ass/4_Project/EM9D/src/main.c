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
#include "hx_drv_iomux.h"
#include "hx_drv_uart.h"
#include "tflitemicro_algo.h"
#include "model_settings.h"
#include "test_samples.h"

#define uart_buf_size 300

DEV_UART * uart0_ptr;

char uart_buf[uart_buf_size] = {0};
char str_buf[uart_buf_size] = {0};
char char_buf[30] = {0};

int8_t pressure[kFeatureSize] = {0};

int main(void)
{
    // Open UART0 only for communication with RPI
    uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);
    uart0_ptr->uart_open(UART_BAUDRATE_115200);

    tflitemicro_algo_init();
    board_delay_ms(1000);

    // Set up blue LED light
    hx_drv_iomux_set_pmux(IOMUX_PGPIO9, 3);
    hx_drv_iomux_set_pmux(IOMUX_PGPIO8, 3);
    hx_drv_iomux_set_outvalue(IOMUX_PGPIO8, 1);
    hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 1);
 
    int count_total = 0;
    int count_single = 0;
    int test_result;

    // Receive a char 's' to check the RPI status with block read
    do {
        uart0_ptr->uart_read(&char_buf, 1);
        board_delay_ms(1);
    } while(char_buf[0] != 's');

    sprintf(uart_buf, "s\r\r\n\0");
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
    board_delay_ms(10);

    while (1) {
        hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 1);     
        uart0_ptr->uart_read(&char_buf, 1);
        hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 0);
       
        /*
            1. If encounter the space, put into the uint_8 list
            2. Otherwise, pad to the string and wait for the space
        */
        if (char_buf[0] != 0x20) {
            // Save the char_buf to string_buf 
            str_buf[count_single] = char_buf[0];
            ++count_single;
        } else {
            str_buf[count_single + 1] = '\0';  
            count_single = 0;
            pressure[count_total] = (signed char)(atoi(str_buf));
            ++count_total;
        }

        // Do the prediction and reset all the count variable
        if (count_total == 8) {
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO8, 0);

            // Predict the result
            test_result = tflitemicro_algo_run(&pressure[0]);  

            sprintf(uart_buf, "%u\r\r\n\0", test_result);
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            board_delay_ms(10);

            // Reset the state
            count_total = 0;
            count_single = 0;
        }
        
        board_delay_ms(1);
    }
    return 0;
}
