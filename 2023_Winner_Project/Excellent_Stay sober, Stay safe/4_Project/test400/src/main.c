#include "max30102.h"
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
//#include "hx_drv_iomux.h"
#include "SC16IS750_Bluepacket.h"
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>

#include "tflitemicro_algo.h"
#include "model_settings.h"

#include "neo6m.h"

#define MAX_SAMPLES 600 //3 sec


DEV_IIC * iic_m0_ptr;

 
float CNN_SQI(uint32_t *PPG1, int len);
double mean(uint32_t *arr, int len);
int find_ssf_peaks(uint32_t *z, int len, int w, int *pks, double threshold);
float BPNN_SQI(float in0, float in1);
void moving_average(uint32_t *signal, int signal_length, int window_size, uint32_t *result);
void ssf(uint32_t *signal, int w, int N, uint32_t *z, double *dsignal);
void process_PPG2_SSF(uint32_t *all_PPG2_values, int total_PPG2_samples);
void PPG1_filter(uint32_t *all_PPG1_values, int total_PPG1_samples);
void PPG2_filter(uint32_t *all_PPG2_values, int total_PPG2_samples);
void flip_signal(uint32_t *signal, int signal_length);
void print_heart_rates(int *pks, double sampling_rate,int *num_peaks);
// void Pca9672_WriteData(uint8_t addr, uint8_t data);
// void Pca9672_Init(void);
int window_size = 25;
uint32_t filtered[MAX_SAMPLES];
uint32_t PPG1_filtered[MAX_SAMPLES];
uint32_t temp;
uint32_t z[MAX_SAMPLES];
uint32_t all_PPG2_values[MAX_SAMPLES];
uint32_t all_PPG1_values[MAX_SAMPLES];
double dsignal[MAX_SAMPLES];
int w = 30 ;
//int start_positions[50];
int foots[50];



float IBI_list[60];
float PPG_mean[50];
int num_foots;
int pks[50];
int fusion[100];
int pks_values[50];
int foots_values[50];
int PK_20[50]; 
int FT_20[50]; 
int PK_FT_fusion_50[60];
int PK_FT_fusion_50_normalize[60]; 
int num_peaks ;
double threshold;
int count = 0;
int ppg_peaks_value_count = 0;
int ppg_foots_value_count = 0;
int P_count = 0;
double height = 0;
int sdnn_flag1=0;
int SDNN_LL;
int Left_hand_flag_L1;
int Left_hand_flag_L2;
int Right_hand_flag_L1;
int Right_hand_flag_L2;

double samples_between_peaks;
double time_between_peaks;
double heart_rate;

float  input_data[1][200][1][1];
uint32_t PPG1[600];
uint32_t PPG2[600];
float  input_data_2[1][50][1][1];
int PPG1_3S;
int PPG2_3S;
int begin_count;
int counter_flash;
int PPG1_samples_null;
int PPG2_samples_null;
float SDNN_50;
int NO_drunk_level;
int drunk_adc;


int Yawn_display = 0;
int Drunk_display = 0;
bool isDrunk = false;

int Left_hand_flag =1;
int Right_hand_flag =1;
int Yawn_flag =0;
int SQI_flag =1;
int Drunk_flag =0;
int average_heart_rate = 0;
//------ ad convert -----------------
volatile static DEV_IIC_PTR dev_iic_m1_ptr;

#define ADC_3021_DEV_ADDR	0x4f
#define REG_ADDR	0x7f

//--------model 1------------------
int  pre_processed_data[200];   
//-----顯示平均心率---
char average_heart_rate_str[10];  // 儲存平均心率字串的 char 陣列

    int total_PPG1_samples = 1;
    int total_PPG2_samples = 1;
    float ppg_predict1 = 0;
    float ppg_predict2 = 0;
    float PPG_mean_this =0;
    float PPG_mean_N =0;
    //int alert_count = 0;
    volatile uint8_t gpio_out_state1 = 0;
    volatile uint8_t gpio_out_state2 = 0;
//------------ screan status ------------------
    static int both_check = -1;
    static int lastShape1 = -1;
    static int lastShape2 = -1;
    int prev_Drunk_display = 0;  // 初始化 prev_Drunk_display 為 0
int on = 1;


int main(void) {
    printf("Starting the program\n");
    //InitUART();

    HX_GPIOSetup();
    IRQSetup();
    UartInit(SC16IS750_PROTOCOL_SPI);
    InitGPIOSetup(SC16IS750_PROTOCOL_SPI);	
    GPIOSetPinMode(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, OUTPUT);
	GPIOSetPinMode(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, OUTPUT);
    GPIOSetPinMode(SC16IS750_PROTOCOL_SPI, CH_A, GPIO1, OUTPUT);
	GPIOSetPinMode(SC16IS750_PROTOCOL_SPI, CH_A, GPIO5, OUTPUT);
    GPIOSetPinMode(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, OUTPUT);//motor
    GPIOSetPinMode(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, OUTPUT);//GPS+BT mtr2

    // iic0_ptr  = hx_drv_i2cm_get_dev(USE_SS_IIC_0);
    // iic0_ptr->iic_open(DEV_MASTER_MODE, IIC_SPEED_STANDARD);//100kbps PCA9672 MAX86150 

	// GPIOSetPinMode(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, OUTPUT);//PCA9672 RESET
	// GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, HIGH);//PCA9672 RES=1
    // board_delay_ms(1); //Delay 120ms

    // Pca9672_Init();
    // Pca9672_WriteData(0x02, 0xFF);

    max30102_1setup();
    max30102_2setup();
    OLED_Init();
    OLED_Clear();   
    OLED_Clear_half('L'); // 清除左半部分   
    OLED_Clear_half('R'); // 清除左半部分                    

    //----------------- mq3 -----------------------------------
    uint8_t send_buf[3];
	uint8_t read_buf[2];
	uint8_t i;
	uint8_t ret_i2c;
	uint8_t reg_buf[2];
	uint8_t dev_addr;
	uint8_t rw_buf[4];

	memset(send_buf, '\0', sizeof(send_buf));
	memset(read_buf, '\0', sizeof(read_buf));
	memset(reg_buf, '\0', sizeof(reg_buf));
	memset(rw_buf, '\0', sizeof(rw_buf));
	memset(reg_buf, '\0', sizeof(reg_buf));
	//memset(rw_buf, '\0', sizeof(rw_buf));
	
	dev_addr =ADC_3021_DEV_ADDR;
	reg_buf[0] = REG_ADDR;

	memset(rw_buf, '\0', sizeof(rw_buf));
	hx_drv_i2cm_read_data(SS_IIC_0_ID, dev_addr, rw_buf, 2);    
    NO_drunk_level = 256*rw_buf[0] + rw_buf[1];
    //-----------------------------------------------------


    //------ Initialize the NEO-6M module -----------------
    //neo6m_init();
    //print_gps_data();

    //--------- oled debug -----------------------
    // int i, j;
    // for(i = 0; i < 128; i += 10) {
    //     for(j = 0; j < 64; j += 10) {
    //        OLED_SetCursor(i, j);
    //       OLED_DisplayString("          w                     w");
    //     }
    // }
    // OLED_Clear_half('R'); // 清除右半部分

    //-- ML init --
    //tflitemicro_algo_init();
    tflitemicro_algo_init();
    tflitemicro_algo_init_2();     
    

    //uint32_t all_PPG1_values[MAX_SAMPLES];
    

    PPG1_3S =0;
    PPG2_3S =0;
    begin_count =0;
    sdnn_flag1=0;
    count=0;
    int samples = 500;
    for(int kk=0;kk<50;kk++)
    {
            PPG_mean[kk]=45000;
            PK_FT_fusion_50[kk]=kk*100;
            
    } //
    for(int kk=0;kk<50;kk++)
    {
            PK_20[kk]=9999;
            FT_20[kk]=9999;
            
    } //    


    while (true) 
    {
            int PPG1_samples = max30102_1loop1(PPG1);
            int PPG2_samples = max30102_2loop1(PPG2);
            //PPG1_3S =0;
            //PPG2_3S =0;
            //-------------------------PPG1---------------------------判斷有無手指
            for (int i = 0; i < PPG1_samples; i++) 
            {
                //printf("PPG1 %d\n", PPG1[i]);
                all_PPG1_values[total_PPG1_samples++] = PPG1[i]/5;
                if (total_PPG1_samples == MAX_SAMPLES) 
                {
                    if(begin_count<20)
                     begin_count = begin_count + 1;
                    PPG1_filter(all_PPG1_values, MAX_SAMPLES);
                    //printf("last sample in array: %d\n",all_PPG1_values[total_samples-1]);
                    ppg_predict1 = CNN_SQI(PPG1_filtered,150); //tflitemicro_algo_run(SQI_CNN)
                    int ppg_predict1_integer = (int)(ppg_predict1 * 1000);
                    int ppg_predict1_decimal = ppg_predict1_integer % 1000;

                     printf("begin_count: %05d\n", begin_count);
                    //printf("SQI prediction: %05d\n", ppg_predict1_integer);
                    printf("PPG1 SQI prediction: %d.%03d\n", ppg_predict1_integer / 1000, ppg_predict1_decimal);
                    if(ppg_predict1 > 0.65 ) //&& PPG1[i] > 1600000)
                        Left_hand_flag_L1 = 1;
                    else 
                        Left_hand_flag_L1 = 0; //Left_hand_flag

                    if(PPG1[i] <800000)
                    {
                        Left_hand_flag=0;
                    }
                    else if( PPG1[i] > 1200000 && (Left_hand_flag_L1==1 || Left_hand_flag_L2==1)) 
                    {
                        Left_hand_flag=1;
                    }
                    else
                    {
                        Left_hand_flag=0;
                    }
                    Left_hand_flag_L2 = Left_hand_flag_L1;
                    // if(PPG1[i] < 2200000)
                    // {  
                    //     //---左手紅燈---
                    //     Left_hand_flag = 0; 
                    //     gpio_out_state1 = 0;
                    //     GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO5, gpio_out_state1);
                    //     gpio_out_state1 = 1;
                    //     GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);
                    //     //GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, gpio_out_state1);
                    //     lastShape1 = 0;
                    // }
                    // else
                    // {
                    //     //---左手綠燈---
                    //     Left_hand_flag =1;
                    //     gpio_out_state1 = 0;
                    //     GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);
                    //     GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, gpio_out_state1);
                    //     gpio_out_state1 = 1;
                    //     GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO5, gpio_out_state1);
                    //     lastShape1 = 1;
                        
                    // }
            
                    total_PPG1_samples = 0;
                    PPG1_3S = 1;
                    // memset(all_PPG1_values, 0, sizeof(all_PPG1_values));
                    memset(all_PPG1_values, 0, sizeof(all_PPG1_values));
                    memset(PPG1_filtered, 0, sizeof(PPG1_filtered));
                }                
		       //----mq3----
		        memset(rw_buf, '\0', sizeof(rw_buf));
		        hx_drv_i2cm_read_data(SS_IIC_0_ID, dev_addr, rw_buf, 2);
                drunk_adc = 256*rw_buf[0] + rw_buf[1];
		        
		        //printf("adc: %02hhx, %02hhx, %3d\r\n", rw_buf[0], rw_buf[1],NO_drunk_level);
		        //if ( rw_buf[0] > (NO_drunk_level) )//0x0c) 
                if ( drunk_adc > (NO_drunk_level+350) )// && begin_count>10)//0x0c)//1800
		        {
                    printf("adc: %02hhx, %02hhx, %3d\r\n", rw_buf[0], rw_buf[1],NO_drunk_level);
		            printf("adc: %5d, %4d\r\n", drunk_adc, NO_drunk_level);
                    Drunk_display = 1; 
		            //---左手紅燈---
		                        gpio_out_state1 = 0;
		                        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO5, gpio_out_state1);
		                        gpio_out_state1 = 1;
		                        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);
		                //---右手紅燈---
		                        gpio_out_state2 = 0;
		                        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO1, gpio_out_state2);
		                        gpio_out_state2 = 1;
		                        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);
		            GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, 0);
		            isDrunk = true;  // 酒駕，設定標記
		            Drunk_flag = 1;
                    //begin_count = 0;
		        }
		        else
		        {   
		            isDrunk = false;  // 沒有酒駕，清除標記
		            Drunk_flag = 0;
		        }                
            }
            //-------------------------PPG2---------------------------#算各種feature
            for (int i = 0; i < PPG2_samples; i++)
            {
                //printf("PPG1 %d\n", PPG2[i]);
                //printf("PPG2 %d\n", PPG2[i]);
                all_PPG2_values[total_PPG2_samples++] = PPG2[i]/5;
                if (total_PPG2_samples == MAX_SAMPLES) 
                {
                    
                    PPG2_filter(all_PPG2_values, MAX_SAMPLES);
                    //printf("last sample in array: %d\n",all_PPG1_values[total_samples-1]);
                    //ppg_predict2 = CNN_SQI(all_PPG2_values,MAX_SAMPLES); // 
                    //ppg_predict2 = CNN_SQI(all_PPG2_values,150); //tflitemicro_algo_run(SQI_CNN)
                    ppg_predict2 = CNN_SQI(filtered,150); //tflitemicro_algo_run(SQI_CNN)
                    int ppg_predict2_integer = (int)(ppg_predict2 * 1000);
                    int ppg_predict2_decimal = ppg_predict2_integer % 1000;

                    //printf("SQI prediction: %05d\n", ppg_predict2_integer);
                    printf("PPG2 SQI prediction: %d.%03d\n", ppg_predict2_integer / 1000, ppg_predict2_decimal);
                    /*if(ppg_predict2 > 0.65 && PPG2[i] > 1600000)
                        Right_hand_flag = 1;
                    else 
                        Right_hand_flag = 0;*/


                    if(ppg_predict2 > 0.65 ) //&& PPG1[i] > 1600000)
                        Right_hand_flag_L1 = 1;
                    else 
                        Right_hand_flag_L1 = 0; //Left_hand_flag

                    if(PPG2[i] <800000)
                    {
                        Right_hand_flag=0;
                    }
                    else if( PPG2[i] > 1200000 && (Right_hand_flag_L1==1 || Right_hand_flag_L2==1)) 
                    {
                        Right_hand_flag=1;
                    }
                    else
                    {
                        Right_hand_flag=0;
                    }
                    Right_hand_flag_L2 = Right_hand_flag_L1;
                    
                    Yawn_flag=0;
                    if(Right_hand_flag==1)
                       process_PPG2_SSF(all_PPG2_values, MAX_SAMPLES); // tflitemicro_algo_run_2 (Yawn_CNN)

                    //----mq3----
                    // memset(rw_buf, '\0', sizeof(rw_buf));
                    // hx_drv_i2cm_read_data(SS_IIC_0_ID, dev_addr, rw_buf, 2);
                    // printf("adc: %02hhx, %02hhx\r\n", rw_buf[0], rw_buf[1]);
                    // if (rw_buf[0] > 0x05) {
                    //     Drunk_display = 1; 
                    //     GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, 1);//red
                    //     GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, 1);//red

                    //     GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, 0);
                    //     isDrunk = true;  // 酒駕，設定標記
                    // } else {
                    //     Drunk_display = 0; 
                    //     GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, 1);
                    //     isDrunk = false;  // 沒有酒駕，清除標記
                    // }
                    //------------
                    
                    // if(PPG2[i] < 2200000)
                    // //if(ppg_predict2 < 0.5)
                    // {
                    //     //---右手紅燈---
                    //     Right_hand_flag =0;
                    //     gpio_out_state2 = 0;
                    //     GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO1, gpio_out_state2);
                    //     gpio_out_state2 = 1;
                    //     GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);
                    //     //GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, gpio_out_state2);
                    //     lastShape2 = 0;
                        
                    // }
                    // else
                    // {
                    //     //---右手綠燈---
                    //     Right_hand_flag =1;
                    //     gpio_out_state2 = 0;
                    //     GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);
                    //     GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, gpio_out_state2);
                    //     gpio_out_state2 = 1;
                    //     GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO1, gpio_out_state2);
                    //     lastShape2 = 1;
                    
                    // }
                    total_PPG2_samples = 0;
                    PPG2_3S =1;
                    memset(all_PPG2_values, 0, sizeof(all_PPG2_values));
                    memset(filtered, 0, sizeof(filtered));
                    memset(z, 0, sizeof(z));
                    memset(dsignal, 0, sizeof(dsignal));
                    memset(foots, 0, sizeof(foots));
                    memset(pks, 0, sizeof(pks));
                    //memset(pks_values, 0, sizeof(pks_values));
                    //ppg_peaks_value_count = 0;
                }
            }
            
  
        // 更新畫面
        if(PPG1_3S==1 && PPG2_3S==1) //(on = 1)
        {
            PPG1_3S=0;
            PPG2_3S=0;
            printf("drunk %d \n",Drunk_flag);
            printf("Yawn %d \n",Yawn_flag);
            printf("SQI %d \n",SQI_flag);
            printf("LH %d \n",Left_hand_flag);
            printf("RH %d \n",Right_hand_flag);
            printf("adc: %5d, %4d\r\n", drunk_adc, NO_drunk_level);
        	if(SQI_flag==1 && Drunk_flag ==1) //&& SDNN_50 >=50 )
        	{
        		//-- Red, Red, motor, GPS --
                //-L-Red
        		gpio_out_state1 = 0;
		        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO5, gpio_out_state1);//L-Green off
		        //gpio_out_state1 = 1;
		        //GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);//L-Red on                  
				//R-Red
		        gpio_out_state2 = 0;
		        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO1, gpio_out_state2);//R-Green off
		        //gpio_out_state2 = 1;
		        //GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);//R-Red on  
		        //motor
                GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, 1);
		        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, 1);//motor on
				//-- GPS      						
                OLED_Clear_half('L');
                OLED_Clear_half('R');
                OLED_SetCursor(3, 12);
                OLED_DisplayString("Drunk detected !!");
                OLED_SetCursor(5, 12);
                OLED_DisplayString("Sending GPS ....");
                //prev_Drunk_display = Drunk_display;  // 更新 prev_Drunk_display 的值為當前 Drunk_display				
                GPIOSetPinMode(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, 0);

                counter_flash=0;
                while(counter_flash<10) //10 sec
                {
                    gpio_out_state1 = 1;
                    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);//L-Red on                     
                    gpio_out_state2 = 1;
                    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);//R-Red on
				    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, 1);//motor on
				    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, 1);//motor on                    
                    board_delay_ms(500);
                    gpio_out_state1 = 0;
                    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);//L-Red off                     
                    gpio_out_state2 = 0;
                    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);//R-Red off               
				    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, 0);//motor off
				    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, 0);//motor off                   
                    board_delay_ms(500);
                    counter_flash = counter_flash + 1;
                    PPG1_samples_null = max30102_1loop1(PPG1);
                    PPG2_samples_null = max30102_2loop1(PPG2);                    
                }
                gpio_out_state1 = 1;
                GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);//L-Red on                     
                gpio_out_state2 = 1;
                GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);//R-Red on              
            }
			else if(SQI_flag==1 && Drunk_flag ==0 && Yawn_flag==1)
			{
				//-- Red, Red, motor
         
		        //motor
				//GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, 1);//motor on
				//GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, 1);//motor on

		        OLED_Clear_half('L');
		        OLED_Clear_half('R');
		        displayCircle();
		        OLED_SetCursor(7, 50);  // 設置 OLED 顯示的位置
		        //OLED_DisplayString("HR: ");
		        //OLED_DisplayString("83");  // 在 OLED 顯示器上顯示平均心率字串
		        //board_delay_ms(4000);
		        OLED_Clear_half('L');
		        OLED_Clear_half('R');
		        OLED_SetCursor(3, 12);
		        OLED_DisplayString("Yawn detected !!");	


                //--Green LED off 
        		gpio_out_state1 = 0;
		        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO5, gpio_out_state1);//L-Green off
 		        gpio_out_state2 = 0;
		        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO1, gpio_out_state2);//R-Green off                               
                //-L-Red R-Red Flashing --
                counter_flash=0;
                while(counter_flash<10) //3 sec
                {
                    gpio_out_state1 = 1;
                    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);//L-Red on                     
                    gpio_out_state2 = 1;
                    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);//R-Red on
				    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, 1);//motor on
				    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, 1);//motor on                    
                    board_delay_ms(250);
                    gpio_out_state1 = 0;
                    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);//L-Red off                     
                    gpio_out_state2 = 0;
                    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);//R-Red off               
				    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, 0);//motor off
				    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, 0);//motor off                   
                    board_delay_ms(250);
                    counter_flash = counter_flash + 1;
                    PPG1_samples_null = max30102_1loop1(PPG1);
                    PPG2_samples_null = max30102_2loop1(PPG2);                    
                }
                gpio_out_state1 = 1;
                GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);//L-Red on                     
                gpio_out_state2 = 1;
                GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);//R-Red on               
            }
			/*else if(SQI_flag==0)
			{
				//-- Red, Red --
                //-L-Red
        		gpio_out_state1 = 0;
		        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO5, gpio_out_state1);//L-Green off
		        gpio_out_state1 = 1;
		        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);//L-Red on                     
				//R-Red
		        gpio_out_state2 = 0;
		        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO1, gpio_out_state2);//R-Green off
		        gpio_out_state2 = 1;
		        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);//R-Red on				
			}*/
			else if(Drunk_flag ==0 && Yawn_flag==0 && Left_hand_flag ==1 && Right_hand_flag ==1  )
			{
                GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, 0);// motor off
                GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, 0);//motor off
				//-- Grrrn, Green, HR --	
				//L-Green	
                gpio_out_state1 = 0;
                GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);//L-Red off
                gpio_out_state1 = 1;
                GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO5, gpio_out_state1);//L-Green on 
				//R-Green				
                gpio_out_state2 = 0;
                GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);//R-Red off
                gpio_out_state2 = 1; 
                GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO1, gpio_out_state2);//R-Green on
				//--Draw circle
                OLED_Clear_half('L');
                OLED_Clear_half('R');
                displayCircle();
                //-- HR --	
                OLED_SetCursor(7, 50);  // 設置 OLED 顯示的位置
                OLED_DisplayString("HR: ");
                OLED_DisplayString(average_heart_rate_str);  // 在 OLED 顯示器上顯示平均心率字串									
			}	
			else if(Drunk_flag ==0 && Yawn_flag==0 && Left_hand_flag ==0 && Right_hand_flag ==0  )
			{
                //GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, 1);//motor on
                //GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, 1);//motor on
                //-- Red, Red --
                //-L-Red
        		gpio_out_state1 = 0;
		        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO5, gpio_out_state1);//L-Green off
		        gpio_out_state1 = 1;
		        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);//L-Red on                     
				//R-Red
		        gpio_out_state2 = 0;
		        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO1, gpio_out_state2);//R-Green off
		        gpio_out_state2 = 1;
		        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);//R-Red on									
                //--Draw Cross
                OLED_Clear_half('L');
                OLED_Clear_half('R');
                displayCross();

                counter_flash=0;
                while(counter_flash<4) //1 sec
                {
                    gpio_out_state1 = 1;
                    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);//L-Red on                     
                    gpio_out_state2 = 1;
                    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);//R-Red on
				    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, 1);//motor on
				    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, 1);//motor on                    
                    board_delay_ms(200);
                    gpio_out_state1 = 0;
                    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);//L-Red off                     
                    gpio_out_state2 = 0;
                    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);//R-Red off               
				    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, 0);//motor off
				    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, 0);//motor off                   
                    board_delay_ms(200);
                    counter_flash = counter_flash + 1;
                    PPG1_samples_null = max30102_1loop1(PPG1);
                    PPG2_samples_null = max30102_2loop1(PPG2);                    
                } 
                gpio_out_state1 = 1;
                GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);//L-Red o 
                gpio_out_state2 = 1;
		        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);//R-Red on                  
            }
			else if(Drunk_flag ==0 && Yawn_flag==0 && Left_hand_flag ==1 && Right_hand_flag ==0  )
			{
           
                //-- Green, Red --		
				//L-Green	
                gpio_out_state1 = 0;
                GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);//L-Red off
                //GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, gpio_out_state1);// motor off
                gpio_out_state1 = 1;
                GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO5, gpio_out_state1);//L-Green on 		
				//R-Red
		        gpio_out_state2 = 0;
		        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO1, gpio_out_state2);//R-Green off
		        gpio_out_state2 = 1;
		        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);//R-Red on								
			    //--Draw Cross
                OLED_Clear_half('L');
                OLED_Clear_half('R');
                displayCross();

                counter_flash=0;
                while(counter_flash<4) //1 sec
                {                    
                    gpio_out_state2 = 1;
                    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);//R-Red on
				    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, 1);//motor on
				    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, 1);//motor on                    
                    board_delay_ms(200);
                    gpio_out_state2 = 0;
                    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);//R-Red off               
				    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, 0);//motor off
				    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, 0);//motor off                   
                    board_delay_ms(200);
                    counter_flash = counter_flash + 1;
                    PPG1_samples_null = max30102_1loop1(PPG1);
                    PPG2_samples_null = max30102_2loop1(PPG2);                    
                }  
                gpio_out_state2 = 1;
                GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);//R-Red on
            }
			else if(Drunk_flag ==0 && Yawn_flag==0 && Left_hand_flag ==0 && Right_hand_flag ==1  )
			{
                //GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, 1);//motor on
                //GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, 1);//motor on
                //-- Red, Green --	
                //-L-Red
        		gpio_out_state1 = 0;
		        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO5, gpio_out_state1);//L-Green off
		        gpio_out_state1 = 1;
		        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);//L-Red on                
				//R-Green				
                gpio_out_state2 = 0;
                GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);//R-Red off
                //GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, gpio_out_state2);//motor off
                gpio_out_state2 = 1; 
                GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO1, gpio_out_state2);//R-Green on							
                //--Draw Cross
                OLED_Clear_half('L');
                OLED_Clear_half('R');
                displayCross();

                counter_flash=0;
                while(counter_flash<4) //1 sec
                {
                    gpio_out_state1 = 1;
                    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);//L-Red on                     
				    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, 1);//motor on
				    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, 1);//motor on                    
                    board_delay_ms(200);
                    gpio_out_state1 = 0;
                    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);//L-Red off                                    
				    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, 0);//motor off
				    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, 0);//motor off                   
                    board_delay_ms(200);
                    counter_flash = counter_flash + 1;
                    PPG1_samples_null = max30102_1loop1(PPG1);
                    PPG2_samples_null = max30102_2loop1(PPG2);                    
                }    
                gpio_out_state1 = 1;
                GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);//L-Red on                    
            }											
        }//end if(PPG1_3S==1 && PPG2_3S==1) 
        
        /*
            on =0;
            PPG1_3S=0;
            PPG2_3S=0;
         //---右手綠燈---
                        gpio_out_state2 = 0;
                        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);//R-Red off
                        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, gpio_out_state2);//motor off
                        gpio_out_state2 = 1; 
                        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO1, gpio_out_state2);//R-Green on

        //---左手綠燈---
                        gpio_out_state1 = 0;
                        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);//L-Red off
                        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, gpio_out_state1);// motor off
                        gpio_out_state1 = 1;
                        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO5, gpio_out_state1);//L-Green on 
                                 
        OLED_Clear_half('L');
        OLED_Clear_half('R');
        displayCircle();
        OLED_SetCursor(7, 50);  // 設置 OLED 顯示的位置
        OLED_DisplayString("HR: ");
        OLED_DisplayString("83");  // 在 OLED 顯示器上顯示平均心率字串
        board_delay_ms(4000);
        OLED_Clear_half('L');
        OLED_Clear_half('R');
        OLED_SetCursor(3, 12);
        OLED_DisplayString("Yawn detected !!");
        Yawn_display =0;

        //---左手紅燈---
        gpio_out_state1 = 0;
        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO5, gpio_out_state1);L-Green off
        gpio_out_state1 = 1;
        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);L-Red on
        //---右手紅燈---
        gpio_out_state2 = 0;
        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO1, gpio_out_state2);R-Green off
        gpio_out_state2 = 1;
        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);R-Red on

        board_delay_ms(10000);
        }
            if(Drunk_display == 1 && prev_Drunk_display != Drunk_display) {
                OLED_Clear_half('L');
                OLED_Clear_half('R');
                OLED_SetCursor(3, 12);
                OLED_DisplayString("Drunk detected !!");
                OLED_SetCursor(5, 12);
                OLED_DisplayString("Sending GPS ....");
                prev_Drunk_display = Drunk_display;  // 更新 prev_Drunk_display 的值為當前 Drunk_display
                Drunk_display = 0;
            }
            // else if(Drunk_display == 0 )
            // {
            //     OLED_Clear_half('L');
            //     OLED_Clear_half('R');
            //     displayCross();
            //     Drunk_display= 2;
            // }
            else if (Yawn_display == 1) {
                OLED_Clear_half('L');
                OLED_Clear_half('R');
                OLED_SetCursor(3, 12);
                OLED_DisplayString("Yawn detected !!");
                Yawn_display =0;

                //---左手紅燈---
                        gpio_out_state1 = 0;
                        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO5, gpio_out_state1);
                        gpio_out_state1 = 1;
                        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state1);
                //---右手紅燈---
                        gpio_out_state2 = 0;
                        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO1, gpio_out_state2);
                        gpio_out_state2 = 1;
                        GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, gpio_out_state2);

                board_delay_ms(3000);
                OLED_Clear_half('L');
                OLED_Clear_half('R');
                displayCircle();
            }

            else if (!isDrunk && lastShape1 == 1 && lastShape2 == 1) {
                // 上次顯示的是叉叉
                if (both_check != 0) {
                    OLED_Clear_half('L');
                    OLED_Clear_half('R');
                    displayCircle();
                    both_check = 0;
                }
                // 更新並顯示心率
                OLED_SetCursor(7, 50);  // 設置 OLED 顯示的位置
                OLED_DisplayString("HR: ");
                OLED_DisplayString(average_heart_rate_str);  // 在 OLED 顯示器上顯示平均心率字串
                
            } 
            else 
            {
                // 上次顯示的是圈圈
                if (!isDrunk && both_check != 1) 
                    {
                        OLED_Clear_half('L');
                        OLED_Clear_half('R');
                        displayCross();
                        both_check = 1;
                    }
                    GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO0, 1);
            }*/
    }//end of while
    return 0;
}

float CNN_SQI(uint32_t *PPG1A, int len) {
    //printf("check0\n");
//len = 200;
 
    // uint32_t *pre_processed_data = malloc(len * sizeof(uint32_t));
    // if (pre_processed_data == NULL) {
    //     printf("Memory allocation error\n");
    //     return;
    // }

    float maxpd = PPG1A[0];
    float minpd = PPG1A[0];
    for (int i = 0; i < len; i++) {
        if (PPG1A[i] > maxpd) {
            maxpd = PPG1A[i];
        }
        if (PPG1A[i] < minpd) {
            minpd = PPG1A[i];
        }
    }

    //float pd_ratio = 3000.0 / (maxpd - minpd);
    for (int i = 0; i < len; i++) {
        pre_processed_data[i] = (PPG1A[i] - minpd)* 3000.0 / (maxpd - minpd);
        //printf("%2d\n",PPG1A[i]);
        //printf("%2f\n",maxpd);
        //printf("%2f\n",minpd);
    }

    // Reshape pre_processed_data to have the shape (1, 50, 1, 1)
    //float  input_data[1][len][1][1];  
    for (int i = 0; i < len; i++) {
        //printf("%5d",pre_processed_data[i]);
        input_data[0][i][0][0] = pre_processed_data[i];
    }
    printf("\n");

    // Pre_processed_data is now ready to be used as input for the model
    //printf("check1\n");
    float test_result = tflitemicro_algo_run(&input_data[0][0][0][0]);
    //board_delay_ms(100);
        if(test_result > 0.65)// && num_peaks>1 && num_foots>1)
        {
            SQI_flag  = 1;
        }
        else
        {
        	SQI_flag   = 0;
		}    
    //printf("check2\n");
    //printf("prediction :%2f\n",test_result);

    // Remember to free the allocated memory when done
    // free(pre_processed_data);
    return test_result;
}




void flip_signal(uint32_t *signal, int signal_length) {
    for (int i = 0; i < signal_length / 2; i++) {
        temp = signal[i];
        signal[i] = signal[signal_length - i - 1];
        signal[signal_length - i - 1] = temp;
    }
}

void moving_average(uint32_t *signal, int signal_length, int window_size, uint32_t *result) {
    for (int i = 0; i < signal_length - window_size + 1; i++) {
        double sum = 0.0;  // 使用 double 类型
        for (int j = i; j < i + window_size; j++) {
            sum += signal[j];
        }
        result[i] = (uint32_t)(sum / window_size);  // 将结果转换回 uint64_t 类型
        //----filter debug----
        //printf("PPG1 %d\n", result[i]);
    }
    // for (int a = 0; a < signal_length - window_size + 1; a++) {
    //     printf("PPG2 %d\n", result[a]);
    // }
}
void ssf(uint32_t *signal, int w, int N, uint32_t *z, double *dsignal) {
    for (int j = 1; j < N; j++) {
        dsignal[j] = (double)signal[j] - (double)signal[j - 1];  // 转换为double以处理负数
        if (dsignal[j] < 0) {
            dsignal[j] = 0;
        }
    }
    for (int i = w; i < N- window_size ; i++) {
        for (int j = i - w + 1; j <= i; j++) {
            z[i] += (uint32_t)dsignal[j];  // 需要把结果转换回uint32_t类型
        }
    }

    //---debug 1----
    // for(int k = 0; k < N; k++)
    //     printf("PPG2 %u\n", signal[k]);
}

// // 自定義一個簡單的結構來保存尖峰數據和相關信息
// typedef struct Peaks {
//     int pks[100];  // 假設最多有100個尖峰
//     int num_peaks;
//     double threshold;
// } Peaks;

// double mean(uint32_t *arr, int len) {
//     double sum = 0.0;
//     for (int i = 0; i < len; i++) {
//         sum += arr[i];
//     }
//     return sum / len;
// }

// Peaks find_ssf_peaks(uint32_t *z, int len, int w) {
//     Peaks peaks = {.num_peaks = 0, .threshold = 1.6 * mean(z, len)};
//     int prev_peak_pos = -1;
    
//     for (int i = 0; i < len - 1; i++) {
//         // 偵測是否大於閾值並且是局部最大值，同時滿足尖峰間距離大於窗口大小
//         if (z[i] > peaks.threshold && z[i] > z[i - 1] && z[i] > z[i + 1] && (prev_peak_pos == -1 || i - prev_peak_pos >= w)) {
//             peaks.pks[peaks.num_peaks++] = i;
//             prev_peak_pos = i;
//         }
//     }
    
//     return peaks;
// }
double mean(uint32_t *arr, int len) {
    double sum = 0.0;
    for (int i = 0; i < len; i++) {
        sum += arr[i];
    }
    return sum / len;
}

// int find_ssf_peaks(uint32_t *z, int len, int w, int *pks, double threshold) {
//     int num_peaks = 0;
//     threshold = 1.8 * mean(z, len);
    
//     int prev_peak_pos = -1;
    
//     for (int i = 0; i < len - 1; i++) {
//         //printf("check i%d\n",i);
//         if (z[i] > threshold && z[i] > z[i - 1] && z[i] > z[i + 1] && (prev_peak_pos == -1 || i - prev_peak_pos >= w)) {
//             pks[num_peaks++] = i;
//             prev_peak_pos = i;
//         }
//     }
//     //printf("Threshold: %f\n", threshold);
//     return num_peaks;
// }
void find_peaks(uint32_t y[], int n, int *pks, int *num_peaks) {
    *num_peaks = 0;
    float height = 1.8 * mean(z, n);
    //height = 300;
    int height_integer = (int)(height * 1000);
    int height_decimal = height_integer % 1000;

    printf("threshold: %d.%03d\n", height_integer / 1000, height_decimal);

    //distance 200(60bps)~100(120bps)
    if (height > 70 && height<1000) 
    {
        if (n > 0 && y[0] > height && y[0] > y[1]) {
            pks[(*num_peaks)++] = 0;
        }
        (*num_peaks) = 0;

        for (int i = 1; i < n - 1; i++) {
            if ((*num_peaks) == 0) {
                if (y[i] >= height && y[i] > y[i - 1] && (y[i] > y[i + 1] || y[i] == y[i + 1])) {
                    printf("distance: %d, %d, %d\n", i, pks[*num_peaks], i - pks[*num_peaks]);
                    pks[(*num_peaks)++] = i;
                }
            } else {
                int distance = i - pks[(*num_peaks) - 1];
                if (y[i] >= height && y[i] > y[i - 1] && (y[i] > y[i + 1] || y[i] == y[i + 1]) && (distance > 75 && distance < 200)) {
                    printf("distance: %d, %d, %d\n", i, pks[(*num_peaks) - 1], distance);
                    pks[(*num_peaks)++] = i;
                }
            }
        }
        if (n > 1 && y[n - 1] > height && y[n - 1] > y[n - 2]) {
            pks[(*num_peaks)++] = n - 1;
        }
    }
}

void find_AC(uint32_t *signal,int *pks,int *num_peaks,int *pks_values,int *foots,int *num_foots,int *foots_values){
    for(int z = 0;z<*num_peaks;z++){
        pks_values[ppg_peaks_value_count++]=signal[pks[z]];
    }
    for(int z = 0;z<*num_foots;z++){
        foots_values[ppg_foots_value_count++]=signal[foots[z]];
    }
}

void print_heart_rates(int *pks, double sampling_rate, int *num_peaks) {
    int total_heart_rate = 0;

    for (int i = 1; i < *num_peaks; i++) {
        int samples_between_peaks = pks[i] - pks[i - 1];
        double time_between_peaks = samples_between_peaks / sampling_rate;
        int heart_rate = 60.0 / time_between_peaks;

        // Splitting the integer and decimal parts
        int heart_rate_integer = heart_rate;
        int heart_rate_decimal = (heart_rate - heart_rate_integer) * 1000;

        IBI_list[count++] = heart_rate_integer;  // 將心率存入 IBI_list 陣列並增加 count
        if (count==50) 
        {
            count=0;
            sdnn_flag1=1;
        }

        total_heart_rate += heart_rate;  // 累加心率值
        printf("Heart rate between peak %d and %d: %d.%03d beats per minute\n", i, i + 1, heart_rate_integer, heart_rate_decimal);

        // char heart_rate_str[10];  // 儲存心率字串的 char 陣列
        // snprintf(heart_rate_str, sizeof(heart_rate_str), "%d.%03d", heart_rate_integer, heart_rate_decimal);

        // OLED_SetCursor(7, 35);  // 設置 OLED 顯示的位置
        // OLED_DisplayString("HR: ");
        // OLED_DisplayString(heart_rate_str);  // 在 OLED 顯示器上顯示心率字串
    }

     average_heart_rate = total_heart_rate / (*num_peaks - 1);  // 計算平均心率

    //printf("Average heart rate: %d beats per minute\n", average_heart_rate);

   if(average_heart_rate == 0)
        snprintf(average_heart_rate_str, sizeof(average_heart_rate_str), "nan");
    else
        snprintf(average_heart_rate_str, sizeof(average_heart_rate_str), "%d", average_heart_rate);
    
}



void find_foots(uint32_t *z, int *maxima, int num_maxima, int *foots, int *num_foots) {
    *num_foots = 0;
    for (int i = 0; i < num_maxima; i++) {
        int j = 1;
        while (j < 15 || z[maxima[i] - j + 1] - z[maxima[i] - j] > 0) {
            j += 1;
        }
        foots[*num_foots] = maxima[i] - j;
        (*num_foots)++;
    }
}

void CNN_YAWN(int *pks_values, int *foots_values, int *fusion) 
{
	/*float ratio;
    if (ppg_peaks_value_count >= 15) {
        
        int max_peak = pks_values[0];
        int min_valley = foots_values[0];

        // 找到最大 peak 和最小 valley
        for (int i = 1; i < 15; i++) {
            if (pks_values[i] > max_peak) {
                max_peak = pks_values[i];
            }
            if (foots_values[i] < min_valley) {
                min_valley = foots_values[i];
            }
        }
       
        ratio = 1000 / (max_peak - min_valley);
        // 正規化 peak 和 valley 到 0 到 1000 的範圍
        for (int i = 0; i < 15; i++) {
            fusion[2 * i] = (pks_values[i] - min_valley) * ratio;
            fusion[2 * i + 1] = (foots_values[i] - min_valley) * ratio;
        }*/
        printf("fusion list: ");
        for(int i = 0; i < 30; i++)
            printf("%d ",fusion[i]);
        printf("\n");

        // Calculate mean
        int sum = 0;
        //int LL=0;
        if(sdnn_flag1==0) SDNN_LL=count;
        else SDNN_LL=50;
        for(int i = 0; i < SDNN_LL; i++) {
            //sum += IBI_list[i];
            sum += 60000/IBI_list[i];
        }
        float mean = (float)sum / (float)SDNN_LL;

        // Calculate standard deviation (SDNN)
        float sum_of_square_diff = 0;
        for(int i = 0; i < SDNN_LL; i++) {
            //sum_of_square_diff += (IBI_list[i] - mean) * (IBI_list[i] - mean);
            sum_of_square_diff += (60000/IBI_list[i] - mean) * (60000/IBI_list[i] - mean);
        }
        //float SDNN = sqrt(sum_of_square_diff / (float)count);
        float SDNN = sqrt(sum_of_square_diff / (float)SDNN_LL);//soso
        SDNN_50 = SDNN;
        
        // Print SDNN in integer format
        int SDNN_integer = (int)(SDNN * 1000);
        int SDNN_integer_part = SDNN_integer / 1000;
        int SDNN_decimal_part = SDNN_integer % 1000;
        //printf("SDNN: %d.%03d (ms)\n", SDNN_integer_part, SDNN_decimal_part);
        printf("SDNN: %d (ms)\n", SDNN_integer_part);

        memset(pks_values, 0, sizeof(pks_values));
        memset(foots_values, 0, sizeof(foots_values));
        //memset(IBI_list, 0, sizeof(IBI_list)); //soso
        ppg_peaks_value_count = 0;
        ppg_foots_value_count = 0;
        //count = 0;
        //Reshape pre_processed_data to have the shape (1, 30, 1, 1)
        
        for (int i = 0; i < 30; i++) {
            //printf("%2d\n",pre_processed_data[i]);
            input_data_2[0][i][0][0] = fusion[i];
        }

        // Pre_processed_data is now ready to be used as input for the model
        //printf("check1\n");
       float test_result = tflitemicro_algo_run_2(&input_data_2[0][0][0][0]);
       int AC1 = PK_FT_fusion_50[0] - PK_FT_fusion_50[28];//soso

        //--soso
        for (int i = 0; i < 30; i++) {
            //printf("%2d\n",pre_processed_data[i]);
            input_data_2[0][i][0][0] = fusion[i+2];
        }

        // Pre_processed_data is now ready to be used as input for the model
        //printf("check1\n");
       float test_result2 = tflitemicro_algo_run_2(&input_data_2[0][0][0][0]);
       int AC2 = PK_FT_fusion_50[2] - PK_FT_fusion_50[30];//soso


       printf("AC1:%2d,  AC2:%2d \n",AC1, AC2);
       //printf("begin_count: %05d\n", begin_count);
       //board_delay_ms(100);
       if(test_result > 0.6 && test_result2>0.6 /*&& AC1>1000 && AC2>1000*/ && begin_count>10) //soso
        {
            Yawn_display = 1;
            Yawn_flag  = 1;
            begin_count =5;
        }
        else if( ((test_result > 0.6  && AC1>1000) || (test_result2>0.6 && AC2>1000)) && begin_count>10 ) 
        {
            Yawn_display = 1;
            Yawn_flag  = 1;
            begin_count =5;
        }        
        else
        {
        	Yawn_display = 0; 
        	Yawn_flag  = 0;
		}
             

        int test_result_integer = (int)(test_result * 1000);
        int test_result_decimal = test_result_integer % 1000;

        int test_result_integer_2 = (int)(test_result2 * 1000);
        int test_result_decimal_2 = test_result_integer_2 % 1000;        

        printf("YAWN prediction: %d.%03d  (%d.%03d)\n", test_result_integer / 1000, test_result_decimal, test_result_integer_2 / 1000, test_result_decimal_2);


}

void PPG1_filter(uint32_t *all_PPG1_values, int total_PPG1_samples) {
 
    //----------------------- MA filter ------------------------
    flip_signal(all_PPG1_values, total_PPG1_samples);
    moving_average(all_PPG1_values,total_PPG1_samples,window_size,PPG1_filtered);
}
void PPG2_filter(uint32_t *all_PPG2_values, int total_PPG2_samples) {
    //----------------------- MA filter ------------------------
    flip_signal(all_PPG2_values, total_PPG2_samples);
    moving_average(all_PPG2_values,total_PPG2_samples,window_size,filtered);
}
void process_PPG2_SSF(uint32_t *all_PPG2_values, int total_PPG2_samples) {
	float maxx, minn, ratio;
    //----------------------- MA filter ------------------------
    //flip_signal(all_PPG2_values, total_PPG2_samples);
    //moving_average(all_PPG2_values,total_PPG2_samples,window_size,filtered);
    //PPG_filter(all_PPG2_values, total_PPG2_samples);
    // for(int s = 0; s < total_PPG2_samples; s++)
    //     printf("PPG1 %d\n", all_PPG2_values[s]);  //打印原始訊號'
    //  for(int k = 0; k < total_PPG2_samples; k++)
    //      printf("PPG1 %d\n", filtered[k]); //打印翻轉過濾波訊號'
    //------------------------ SSF ----------------------------
    ssf(filtered, w, total_PPG2_samples, z, dsignal);
    //  for(int k = 0; k < total_PPG2_samples; k++)
    //  {
    //      //printf("PPG2 position: %d , value %d\n",k ,z[k]); //打印SSF訊號
    //      printf("PPG2 %d\n",z[k]); //打印SSF訊號
    //  }
    //-------------------- Find Peaks ---------------------------
    //num_peaks = find_ssf_peaks(z, total_PPG2_samples - window_size, w, pks, threshold);
    find_peaks(z,total_PPG2_samples,pks,&num_peaks);
    //printf("threshold: %4.1f\n",height);
    

    // for(int i = 0; i < num_peaks; i++)
    //      printf("Peak %d: %d\n", i + 1, pks[i]);  // 打印所有的尖峰位置
    print_heart_rates(pks, 200,&num_peaks);  // 打印每兩個相鄰尖峰之間的心率
    


    //---------------------- Find foots ----------------------------
    find_foots(z, pks, num_peaks, foots, &num_foots);
    // for(int i = 0; i < num_foots; i++)
    //     printf("Foot %d: %d\n", i + 1, foots[i]);  // 打印所有的腳點位置
    //--------------------- pre-processed ---------------------------
    find_AC(all_PPG2_values,pks,&num_peaks,pks_values,foots,&num_foots,foots_values);
    //-- push-in-out PK_20, FT_20 ---  
    /*for (int i = 0; i < 20; i++) 
	{
		//-- PK --
		if( i<(20-num_peaks) ) 	PK_20[i]=PK_20[i+num_peaks];
		else PK_20[i] = pks_values[i-(20-num_peaks)];
		//-- FT --
		if( i<(20-num_foots) ) 	FT_20[i]=FT_20[i+num_foots];
		else FT_20[i] = foots_values[i-(20-num_foots)];				
	}*/
	for (int i = 25-1; i >=0; i--) //soso
	{
		//-- PK --
		if(i>=num_peaks) 	PK_20[i]=PK_20[i-num_peaks];
		else PK_20[i] = pks_values[num_peaks-i-1];
		//-- FT --
		if( i>=num_foots) 	FT_20[i]=FT_20[i-num_foots];
		else FT_20[i] = foots_values[num_foots-i-1];				
	}    
    //---- select   15 PK, 15 Ft into PK_FT_fusion_50 -- 
    for (int i = 0; i < 20; i++) 
	{
		PK_FT_fusion_50[2*i] = PK_20[i];
		PK_FT_fusion_50[2*i+1] = FT_20[i];
	}    
	//-- PK_FT_fusion_50_normalize --
	maxx = -99999999;
	minn = 99999999;
	
    for (int i = 0; i < 40; i++) 
	{
        if (PK_FT_fusion_50[i] > maxx)    maxx = PK_FT_fusion_50[i];
        if (PK_FT_fusion_50[i] < minn)    minn = PK_FT_fusion_50[i];
    }
       
    ratio = 1000 / (maxx - minn);
    //---normalization
    for (int i = 0; i < 40; i++) 
	{
        PK_FT_fusion_50_normalize[i] = (PK_FT_fusion_50[i] - minn) * ratio;
        if(num_peaks==0 || num_foots==0)
           PK_FT_fusion_50_normalize[i]=0;
    }	
	
    //------
    //CNN_YAWN(PK_20,FT_20,PK_FT_fusion_50);  //(pks_values,foots_values,fusion); 
    

    //All Peaks
    //printf("Peaks: ");
    // for(int i = 0; i < num_peaks; i++)
    //     printf("Peaks: %d value: %d\n", pks[i],pks_values[i]);
    // printf("\n");
    printf("peaks value list: ");
    for(int i = 0; i < ppg_peaks_value_count; i++)
        printf("%d ",pks_values[i]);
    printf("\n");

    printf("foots value list: ");
    for(int i = 0; i < ppg_foots_value_count; i++)
        printf("%d ",foots_values[i]);
    printf("\n");
    
    
    printf("Heart rate list: ");
    for (int i = 0; i < SDNN_LL; i++) //count
    {
        int heart_rate_integer = (int) (IBI_list[i]);
        printf("%d ", heart_rate_integer);
    }
    printf("\n");

    CNN_YAWN(pks_values,foots_values,PK_FT_fusion_50_normalize);

    // // All Foots
    // printf("Foots: ");
    // for(int i = 0; i < num_foots; i++)
    //     printf("%d ", foots[i]);
    // printf("\n");

}

// void Pca9672_Init(void) {
//     Pca9672_WriteData(0x00, 0x06); // Software Reset
//     Pca9672_WriteData(0x02, 0x00); // 设置P2为输出模式
//     Pca9672_WriteData(28, 0x00);
//     board_delay_ms(10);
// }

// void Pca9672_WriteData(uint8_t addr, uint8_t data) {
//     uint8_t WriteData[2];
//     WriteData[0] = addr;
//     WriteData[1] = data;
    
//     if (data == 0x00) {
//         hx_drv_i2cm_write_data(USE_SS_IIC_X, PCA9672_ADDRESS, WriteData, 2, NULL, 0);
//     } else {
//         hx_drv_i2cm_write_data(USE_SS_IIC_X, PCA9672_ADDRESS, WriteData, 2, WriteData, 1);
//     }
// }
