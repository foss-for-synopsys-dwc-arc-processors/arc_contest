/**************************************************************************************************
    (C) COPYRIGHT, WEN YAN CHENG. ALL RIGHTS RESERVED
    ------------------------------------------------------------------------
    File        : main.c
    Project     : WOLO
    DATE        : 2023/5/29
    AUTHOR      : CHENG
    BRIFE       : main function
    HISTORY     : Initial version - 2023/5/29 created by Cheng
**************************************************************************************************/
#include "main.h"
#include "tflitemicro_algo.h"
#include "model_settings.h"

#include "synopsys_sdk_camera_drv.h"

// For wifi module
#include "SC16IS750_Bluepacket.h"

extern uint32_t g_wdma2_baseaddr;

// uint8_t test_img[kNumRows * kNumCols] = {0};
// int8_t test_img [kNumRows * kNumCols] = {0};
uint8_t output_img [kNumRows * kNumCols] = {0};
// uint8_t output_height = kNumRows;
// uint8_t output_width = kNumCols;

DEV_UART * uart0_ptr;
DEV_UART * uart1_ptr;
char str_buf[100];
extern int8_t cmd_buf[256];
// uint8_t cmd_buf[100];
// #define TEST_UPPER 25
// uint8_t test_cnt = 0;
// uint8_t test_correct = 0;
// #include "test_samples.h"

// For wifi module
void hx_uart_init()
{
	// printf("into %s-%d\r\n", __func__, __LINE__);
	uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);
	uart1_ptr = hx_drv_uart_get_dev(USE_SS_UART_1);
	uart0_ptr->uart_open(UART_BAUDRATE_115200); //UART0 can't change baud
	uart1_ptr->uart_open(UART_BAUDRATE_115200); //UART1
	//uart0_ptr->uart_write(startup_info, strlen(startup_info));
    //uart1_ptr->uart_write(startup_info, strlen(startup_info));

	printf("UART1 T Mode : %d\r\n", uart1_ptr->uart_info.transfer_mode);

	return ;
}
void httpParse(char *c_fp, uint8_t interface, uint16_t len){
    while(len--){
        parsing_char(c_fp++, interface);
    }
}

void HttpPostInit(uint8_t interface){
    memset(cmd_buf, '\0', sizeof(cmd_buf));

    sprintf(str_buf, "AT+HTTPPOH=0,\"Content-Type:text/plain\"\r\n\0");
    httpParse(str_buf, interface, strlen(str_buf));
    board_delay_ms(1000);

    sprintf(str_buf, "AT+HTTPPOU=0,\"http://192.168.2.101:8000\"\r\n\0");
    httpParse(str_buf, interface, strlen(str_buf));
    board_delay_ms(1000);
}

void httpPostTest(uint8_t interface, int i){
	sprintf(str_buf, "AT+HTTPPOD=14\r\n\0");
    httpParse(str_buf, interface, strlen(str_buf));
    board_delay_ms(300);

	sprintf(str_buf, "HelloCodeTestAAAA%d=\r\n\0", i);
    httpParse(str_buf, interface, strlen(str_buf));
    board_delay_ms(300);


    sprintf(str_buf, "AT+HTTPPO=0\r\n\0");
    httpParse(str_buf, interface, strlen(str_buf));
    board_delay_ms(300);
}

char msg_buf[100];
char http_post[3][100] = {"AT+HTTPPOD=1\r\n\0", "\0", "AT+HTTPPO=0\r\n\0"};
char pred_msg[200];
void httpPostMsg(uint8_t interface, char *msg, uint16_t msg_len){
	sprintf(str_buf, "AT+HTTPPOD=2\r\n\0");
    httpParse(str_buf, interface, strlen(str_buf));
    board_delay_ms(300);

	// sprintf(str_buf, "%s=\r\n\0", msg);
    httpParse(msg_buf, interface, strlen(msg_buf));
    board_delay_ms(300);


    sprintf(str_buf, "AT+HTTPPO=0\r\n\0");
    httpParse(str_buf, interface, strlen(str_buf));
    board_delay_ms(300);
}
// pred = xyxy, conf, dist, cls
extern float pred[max_bbox][7];
extern int8_t pred_idx;
char tmp_pred[10];

void convertPred(){
    sprintf(pred_msg, "");
    for(int i=0;i<pred_idx;i++){
        int x = (int)((float)(pred[i][0] + pred[i][2]) / 2.0);
        int dist = (int)pred[i][5];

        // the send obj is seperate with '-', and with form "center_x,dist"
        sprintf(tmp_pred, "%d,%d-", x, dist);
        strcat(pred_msg, tmp_pred);
    }

    // add end flag
    sprintf(tmp_pred, "=\r\n\0");
    strcat(pred_msg, tmp_pred);
    pred_idx = 0;
}

void sendCmd(uint8_t interface, char *msg){
    httpParse(msg, interface, strlen(msg));
}

// end wifi module
int main(void)
{
    // For wif module
	hx_uart_init();
    HX_GPIOSetup();
	IRQSetup();
	UartInit(SC16IS750_PROTOCOL_SPI);
    board_delay_ms(1000);
    printf("\n\nStart WOLO system\n\n");

    synopsys_camera_init();
    tflitemicro_algo_init();
    HttpPostInit(SC16IS750_PROTOCOL_SPI);
    
    while(1)
    {
        for(int i=0;i<3;i++){
            char *send_cmd;
            if( i == 1){
                convertPred();
                send_cmd = pred_msg;
            } 
            else
                send_cmd = http_post[i];
            sendCmd(SC16IS750_PROTOCOL_SPI, send_cmd);
            
            
            synopsys_camera_start_capture();
            board_delay_ms(100);

            uint8_t * img_ptr;
            uint32_t img_width = 640;
            uint32_t img_height = 480;
            img_ptr = (uint8_t *) g_wdma2_baseaddr;

            synopsys_camera_down_scaling(img_ptr, img_width, img_height, &output_img[0], 320, 240);
            
            int32_t test_result = tflitemicro_algo_run(&output_img[0]);
            if(test_result != 0)
                printf("Error code: %2d\r\n\n", test_result);
            else
                printf("Success!!!!!\n\n");
        }
    }
    

	return 0;
}

