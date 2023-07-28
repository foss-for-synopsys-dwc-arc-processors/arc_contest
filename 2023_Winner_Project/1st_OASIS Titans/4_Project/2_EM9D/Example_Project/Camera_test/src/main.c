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
#include "main.h"

#include "synopsys_sdk_camera_drv.h"

#define original_image_height (480)
#define original_image_width (640)

#define output_height (100) // /8 //224
#define output_width (100) // /8 //224

DEV_UART * uart0_ptr;
char uart_buf[uart_buf_size] = {0};

extern uint32_t g_wdma2_baseaddr;

uint8_t output_img [output_height * output_width] = {0};

int main(void)
{
    //UART 0 is already initialized with 115200bps
    uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);

	printf("This is Lab3_Gray_Camera\r\n");
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(10);

    /*inital*/
    synopsys_camera_init();

    sprintf(uart_buf, "Wait for user press key: [A] \r\n");    
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(10);    
    while(1)
    {
        uint8_t user_key = 0;
        uint8_t function_ret = uart0_ptr->uart_read_nonblock(&user_key, 1);
        if(function_ret != 1)
            user_key = 0;

        if(user_key == 'A')
        {     
            sprintf(uart_buf, "Start_to_Capture\r\n");   
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);
            synopsys_camera_start_capture();
            board_delay_ms(100);

            sprintf(uart_buf, "Camera Get Data Success\r\n");   
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);

            uint8_t * img_ptr;
            uint32_t img_width = original_image_width;
            uint32_t img_height = original_image_height;
            img_ptr = (uint8_t *) g_wdma2_baseaddr;

            sprintf(uart_buf, "Start to send\r\n");   
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);

            sprintf(uart_buf, "Image width: %d\r\n", img_height);   
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);
    
            sprintf(uart_buf, "Image height: %d\r\n", img_width);   
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);
    
            sprintf(uart_buf, "Image size: %d Bytes\r\n", img_height * img_width);   
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);

            sprintf(uart_buf, "Image address: 0x%08x\r\n", img_ptr);   
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);

            // output_height = img_height / 4;
            // output_width = img_width / 4;
            synopsys_camera_down_scaling(img_ptr, img_width, img_height, &output_img[0], output_width, output_height);

            sprintf(uart_buf, "Scaling image width: %d\r\n", output_width);   
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);
    
            sprintf(uart_buf, "Scaling image height: %d\r\n", output_height);   
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);
    
            sprintf(uart_buf, "Scaling image size: %d Bytes\r\n", output_height * output_width);   
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);

            sprintf(uart_buf, "Scaling image address: 0x%08x\r\n", &output_img[0]);   
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);

            for(uint32_t heigth_cnt = 0; heigth_cnt < output_height; heigth_cnt ++)
            {
                for(uint32_t width_cnt = 0; width_cnt < output_width; width_cnt ++)
                {                        
                    if(width_cnt != (output_width - 1))
                        sprintf(uart_buf, "%3d, ", output_img[(heigth_cnt * output_width) + width_cnt]);
                    else
                        sprintf(uart_buf, "%3d\r\n", output_img[(heigth_cnt * output_width) + width_cnt]);

                    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                        board_delay_ms(1);  
                }
            }
            sprintf(uart_buf, "End of send\r\n\n");  
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);    

            sprintf(uart_buf, "Wait for user press key: [A] \r\n");    
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);    
        }

        board_delay_ms(10);
    }
}

