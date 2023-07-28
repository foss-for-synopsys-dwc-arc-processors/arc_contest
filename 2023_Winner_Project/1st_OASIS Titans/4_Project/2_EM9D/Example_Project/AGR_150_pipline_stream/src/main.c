/***************************************************************************************************/
// FILE NAME:    main.cpp
// VERSION:      1.0 
// DATE:         July 1, 2023
// AUTHOR:       KUAN-WEI, CHEN, NYCU IEE
// CODE TYPE:    CPP
// DESCRIPTION:  2023 Synopsys ARC AIoT / Analog Gauge Reader
// MODIFICATION: 
// Date          Description
// 07/01       
/***************************************************************************************************/
#include "main.h"

#include "tflitemicro_algo.h"
#include "model_settings.h"


#include "synopsys_sdk_camera_drv.h"
#include "hx_drv_iic_m.h"
#include "synopsys_i2c_oled1306.h"

DEV_UART * uart0_ptr;
DEV_IIC * iic1_ptr;

char uart_buf[uart_buf_size] = {0};
char str_buf[50] = {0};

extern uint32_t g_wdma2_baseaddr;

int8_t test_img [kNumRows * kNumCols] = {0};
uint8_t output_img [kNumRows * kNumCols] = {0};
uint8_t output_height = kNumRows;
uint8_t output_width = kNumCols;

uint8_t * img_ptr;
uint32_t img_width = 640;
uint32_t img_height = 480;

uint8_t state = 0; //0: IDLE, 1: Adjust, 2: Inference
uint32_t timing_sec_cnt;


int main(void)
{
    //UART 0 is already initialized with 115200bps
    // uart0 init
    uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);

    // i2c init
    iic1_ptr = hx_drv_i2cm_get_dev(USE_SS_IIC_1);
    iic1_ptr->iic_open(DEV_MASTER_MODE, IIC_SPEED_STANDARD); 
    OLED_Init();

	OLED_Clear();                        
	OLED_SetCursor(0, 0);
    OLED_DisplayString("Welcome To OASIS Lab");

    // GPIO init
    hx_drv_iomux_set_pmux(IOMUX_PGPIO8, 3); //Extension Board Red LED Output
    hx_drv_iomux_set_pmux(IOMUX_PGPIO9, 3); //Extension Board Blue LED Output
    hx_drv_iomux_set_pmux(IOMUX_PGPIO12, 3); //Extension Board Green LED Output

    hx_drv_iomux_set_outvalue(IOMUX_PGPIO8, 1);
    hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 0);
    hx_drv_iomux_set_outvalue(IOMUX_PGPIO12, 0);
    
    // Camera
    synopsys_camera_init();
    
    // TFLM model
    tflitemicro_algo_init();
    tflitemicro_algo_init1();


    sprintf(uart_buf, "**==========================================================**\r\n"); uart0_ptr->uart_write(uart_buf, strlen(uart_buf)); board_delay_ms(50);
    sprintf(uart_buf, "**       .88888.    .d888888  .d88888b   dP  .d88888b       **\r\n"); uart0_ptr->uart_write(uart_buf, strlen(uart_buf)); board_delay_ms(50);
    sprintf(uart_buf, "**      d8a   a8b  d8a    88  88a    'a  88  88a    ''      **\r\n"); uart0_ptr->uart_write(uart_buf, strlen(uart_buf)); board_delay_ms(50);
    sprintf(uart_buf, "**      88     88  88aaaaa88  'Y88888b.  88  `Y88888b.      **\r\n"); uart0_ptr->uart_write(uart_buf, strlen(uart_buf)); board_delay_ms(50);
    sprintf(uart_buf, "**      88     88  88     88        `8b  88        `8b      **\r\n"); uart0_ptr->uart_write(uart_buf, strlen(uart_buf)); board_delay_ms(50);
    sprintf(uart_buf, "**      Y8.   .8P  88     88  d8'   .8P  88  d8'   .8P      **\r\n"); uart0_ptr->uart_write(uart_buf, strlen(uart_buf)); board_delay_ms(50);
    sprintf(uart_buf, "**       `8888P'   88     88   Y88888P   dP   Y88888P       **\r\n"); uart0_ptr->uart_write(uart_buf, strlen(uart_buf)); board_delay_ms(50);
    sprintf(uart_buf, "**                                                          **\r\n"); uart0_ptr->uart_write(uart_buf, strlen(uart_buf)); board_delay_ms(50);
    sprintf(uart_buf, "**    d888888b  d8888b  d888888b  .d888888   .d88    8d     **\r\n"); uart0_ptr->uart_write(uart_buf, strlen(uart_buf)); board_delay_ms(50);
    sprintf(uart_buf, "**       88       88       88     d8a    88  d8 P8   8d     **\r\n"); uart0_ptr->uart_write(uart_buf, strlen(uart_buf)); board_delay_ms(50);
    sprintf(uart_buf, "**       88       88       88     88aaaaa88  d8  P8  8d     **\r\n"); uart0_ptr->uart_write(uart_buf, strlen(uart_buf)); board_delay_ms(50);
    sprintf(uart_buf, "**       88       88       88     88     88  d8   P8 8d     **\r\n"); uart0_ptr->uart_write(uart_buf, strlen(uart_buf)); board_delay_ms(50);
    sprintf(uart_buf, "**       88       88       88     88     88  d8    P88d     **\r\n"); uart0_ptr->uart_write(uart_buf, strlen(uart_buf)); board_delay_ms(50);
    sprintf(uart_buf, "**       88     d8888b     88     88     88  d8     P8d     **\r\n"); uart0_ptr->uart_write(uart_buf, strlen(uart_buf)); board_delay_ms(50);
    sprintf(uart_buf, "**==========================================================**\r\n"); uart0_ptr->uart_write(uart_buf, strlen(uart_buf)); board_delay_ms(50);
    
    sprintf(uart_buf, " Press [A] to Adjust Mode \r\n"); uart0_ptr->uart_write(uart_buf, strlen(uart_buf)); board_delay_ms(50);
    sprintf(uart_buf, " Press [I] to Inference Mode \r\n"); uart0_ptr->uart_write(uart_buf, strlen(uart_buf)); board_delay_ms(50);
    
    while(1)
    {
        // Capture Image
        synopsys_camera_start_capture();
        board_delay_ms(10); // 100

        // Image Down Scaling
        img_ptr = (uint8_t *) g_wdma2_baseaddr;

        synopsys_camera_down_scaling(img_ptr, img_width, img_height, &output_img[0], output_width, output_height);
     
        for(uint32_t pixel_index = 0; pixel_index < kImageSize; pixel_index++)
            test_img[pixel_index] = output_img[pixel_index] - 128;

        int32_t test_result = tflitemicro_algo_run(&test_img[0]);
        int32_t test_result1 = tflitemicro_algo_run1(&test_img[0]);


        // Keyboard
        uint8_t user_key = 0;
        uint8_t function_ret = uart0_ptr->uart_read_nonblock(&user_key, 1);

        if(function_ret != 1)
            user_key = 0;
        

        // Idle state
        if(state == 0){
            if(user_key == 'A'){
                sprintf(uart_buf, "[Adjust Mode] \r\n"); uart0_ptr->uart_write(uart_buf, strlen(uart_buf)); board_delay_ms(50);
                state = 1;
            }
            else if(user_key == 'I' | (state == 0 && timing_sec_cnt > 15)){
                sprintf(uart_buf, "[Inference Mode] \r\n"); uart0_ptr->uart_write(uart_buf, strlen(uart_buf)); board_delay_ms(50);
                state = 2;
            }
        }
        
        // Adjust State
        if(state == 1){ 
            sprintf(uart_buf, "/%d\r\n", test_result1);
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(1);


            sprintf(uart_buf, "Send image, please wait...\n<\r\n");
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(1);
            
            // sprintf(uart_buf, "<\r\n");
            // uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            //     board_delay_ms(10);

            for(uint32_t heigth_cnt = 2; heigth_cnt < output_height; heigth_cnt = heigth_cnt + 5)
            {
                for(uint32_t width_cnt = 2; width_cnt < output_width; width_cnt = width_cnt + 5)
                {                    
                    if((width_cnt % 5 == 2) && (heigth_cnt % 5 == 2)){
                        // if( (heigth_cnt == output_height - 1) && (width_cnt == output_width - 1) ){
                        //     sprintf(uart_buf, "%d", output_img[(heigth_cnt * output_width) + width_cnt]);
                        // }
                        if( (heigth_cnt == output_height - 3) && (width_cnt == output_width - 3) ){
                            sprintf(uart_buf, "%d", output_img[(heigth_cnt * output_width) + width_cnt]);
                        }
                        else{
                            sprintf(uart_buf, "%d,", output_img[(heigth_cnt * output_width) + width_cnt]);
                        }

                        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                        board_delay_ms(1); 
                    }
                    
                    
                   
                    // if(width_cnt != (output_width - 1))
                    //     sprintf(uart_buf, "%3d, ", output_img[(heigth_cnt * output_width) + width_cnt]);
                    // else
                    //     sprintf(uart_buf, "%3d\r\n", output_img[(heigth_cnt * output_width) + width_cnt]);

                    // uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                    //     board_delay_ms(1);  
                }
            }
            sprintf(uart_buf, ">\r\n");
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(1);
        }
        

        


        // Inference State
        if(state == 2){ 
            sprintf(uart_buf, "%2d\r\n", test_result);
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            board_delay_ms(10);  
        }
        

        OLED_SetCursor(1, 0);
        OLED_DisplayString("--------------------");

        OLED_SetCursor(2, 0);
        sprintf(str_buf, "Timer: %4d sec", timing_sec_cnt);
        OLED_DisplayString(str_buf);

        OLED_SetCursor(3, 0);
        sprintf(str_buf, "Detection: %2d", test_result1);
        OLED_DisplayString(str_buf);

        OLED_SetCursor(4, 0);
        sprintf(str_buf, "Prediction: %2d", test_result);
        OLED_DisplayString(str_buf);

        OLED_SetCursor(5, 0);
        int32_t upper = test_result*5+5;
        int32_t lower = test_result*5;
        sprintf(str_buf, "Degree: %3d - %3d", lower, upper);
        OLED_DisplayString(str_buf);


        OLED_SetCursor(7, 0);
        if(test_result1 == 0){
            OLED_DisplayString("Not detected!       ");
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO8, 0);
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 0);
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO12, 0);
        }
        else if(test_result1 == 1){
            OLED_DisplayString("Shift to upper-left "); // On the upper-left
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO8, 1);
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 1);
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO12, 0);
        }
        else if(test_result1 == 2){
            OLED_DisplayString("Shift to upper      "); // On the upper
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO8, 1);
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 1);
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO12, 0);
        }
        else if(test_result1 == 3){
            OLED_DisplayString("Shift to upper-right"); // On the upper-right
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO8, 1);
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 1);
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO12, 0);
        }
        else if(test_result1 == 4){
            OLED_DisplayString("Shift to left       "); // On the left
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO8, 1);
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 1);
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO12, 0);
        }
        else if(test_result1 == 5){
            OLED_DisplayString("Detected !!!        ");
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO8, 1);
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 0);
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO12, 1);
        }
        else if(test_result1 == 6){
            OLED_DisplayString("Shift to right      "); // On the right
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO8, 1);
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 1);
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO12, 0);
        }
        else if(test_result1 == 7){
            OLED_DisplayString("Shift to lower-left "); // On the lower-left
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO8, 1);
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 1);
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO12, 0);
        }
        else if(test_result1 == 8){
            OLED_DisplayString("Shift to lower      "); // On the lower
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO8, 1);
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 1);
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO12, 0);
        }
        else{
            OLED_DisplayString("Shift to lower-right"); //On the lower-right
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO8, 1);
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 1);
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO12, 0);
        }


        board_delay_ms(1);



        timing_sec_cnt ++;
        //board_delay_ms(10); // 1000
    }

	return 0;
}

