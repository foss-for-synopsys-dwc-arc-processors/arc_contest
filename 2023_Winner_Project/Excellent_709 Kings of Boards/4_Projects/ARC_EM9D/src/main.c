/*************************************************************
* This code will predict the RGB image receive from Raspi

* The RGB image will be the input of ResNet tflite model
* The output of ResNet tflite model will be sent back to Raspi

* 2023/7/8 by 709
**************************************************************/


#include "main.h"
#include "tflitemicro_algo.h"
#include "model_settings.h"


// init uart pointer
DEV_UART * uart0_ptr;
DEV_UART * uart1_ptr;

// UART receive buffer
char uart_buffer[uart_buf_size] = {0};
char read_buffer[uart_buf_size] = {0};

// Assuming RGB image with kImageSize pixels
uint8_t image_data[kImageSize] = {0};  



int main(void)
{
    // looping keep looping
    while(1){
        printf("In main\r\n");

        // define uart pointer
        uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);
        uart1_ptr = hx_drv_uart_get_dev(USE_SS_UART_1);
        uart0_ptr->uart_open(UART_BAUDRATE_115200);
        uart1_ptr->uart_open(UART_BAUDRATE_115200);
            board_delay_ms(10);

        // initializie the tflite model
        printf("tflitemicro_algo_init\r\n");
        tflitemicro_algo_init();

        int counter = 0;
        int flag = 0;
        bool send_start = false;
        bool stop_flag = false;

        printf("Start recieving picture\r\n");
            board_delay_ms(10);

        // Recieve the RGB image from Raspi
        while(1){

            // read the image all at once
            flag = uart0_ptr->uart_read(&image_data, kImageSize);

            if (flag != 0) {
                // if get the image successfully
            
                for (int i=0; i<10; i++) {
                    printf("%d, ", image_data[i]);
                }

                // debug line
                printf("\n");
                printf("get size is :%d\r\n", strlen(image_data));
                printf("counter = %d\n", counter);

                board_delay_ms(20);
                
                break;
            }
            
        } // end of while
        board_delay_ms(30);

        // printf("After Em9d get the picture\n");

        // Predict the image using ResNet tflite model
        int test_result = tflitemicro_algo_run(&image_data[0]);
            board_delay_ms(1000);

        // send the predict result to uart1 until Raspi recieve
        while(1){

            // send the result to uart1
            sprintf(uart_buffer, "%d\n", test_result);
            uart1_ptr->uart_write(uart_buffer, strlen(uart_buffer));
            board_delay_ms(30);

            // check if Raspi gets the result
            int get_result = uart0_ptr->uart_read_nonblock(&read_buffer, 1);

            if (get_result == 1){
                // if raspi get the result
                // printf("get from raspi d: %d\r\n", read_buffer[0]);
                // printf("get from raspi c: %c\r\n", read_buffer[0]);

                if (read_buffer[0] == 0){
                    printf("Raspi has got the result, stop sending\r\n");
                    break;
                }
            }
            // printf("type of read:", typeof(read_buffer[0]));
        }// end of while

        board_delay_ms(1000);

    } // end of main while
    
    return 0;
}
