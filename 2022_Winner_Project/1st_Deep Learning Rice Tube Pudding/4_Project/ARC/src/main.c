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

#include "hx_drv_uart.h"

#include "tflitemicro_algo.h"
#include "model_settings.h"
#include "max86150.h"
#include "synopsys_i2c_oled1306.h"
#include "synopsys_sdk_GMA303KU.h"
#include "data_processing.h"

#define limit -370

int16_t accel_x;
int16_t accel_y;
int16_t accel_z;
int16_t accel_t;

float ECG[MAX] = {0};
float BufECG[MAX - extra] = {0};
float NewECG[(MAX - extra) / 2] = {0};
float coeff_bandfilter[40] = {
    -0.0012,   -0.0018,   -0.0005,    0.0018,    0.0012,
    -0.0040,   -0.0077,   -0.0017,    0.0088,    0.0062,
    -0.0138,   -0.0257,   -0.0035,    0.0321,    0.0242,
    -0.0413,   -0.0852,   -0.0048,    0.1947,    0.3695,
    0.3695,    0.1947,   -0.0048,   -0.0852,   -0.0413,
    0.0242,    0.0321,   -0.0035,   -0.0257,   -0.0138,
    0.0062,    0.0088,   -0.0017,   -0.0077,   -0.0040,
    0.0012,    0.0018,   -0.0005,   -0.0018,   -0.0012,
    };


int main(void)
{    

	tflitemicro_algo_init();
    uint8_t chip_id = GMA303KU_Init();
    board_delay_ms(100);

    OLED_Init();    
    OLED_Clear();
	OLED_SetCursor(0, 0);

    if(chip_id == 0xA3)
        DisplayResult('R');
    else
        OLED_DisplayString("GMA303KU_Init Error");
    board_delay_ms(10);

    while(1){
        uint16_t reg_04_data = GMA303KU_Get_Data(&accel_x, &accel_y, &accel_z, &accel_t);

        if(accel_z < limit ){
            DisplayResult('A');
            DisplayResult('N');
            break;
        }
        board_delay_ms(10);
    }

    int min, yes, hr;
    min = 0;
    yes = 0;
    hr = 0;
    InitUART();
    InitMax86150();

   
    while (1)
    {
        int index;
        int number = GetECGloop(ECG);

        if(number != MAX ){
            break;
        }

        Filter(ECG, coeff_bandfilter, BufECG);
        DownSample(BufECG, NewECG);
        Normalization(NewECG);

       

        index = tflitemicro_algo_run(NewECG);
        DisplayResult(kCategoryLabels[index]);
        if(kCategoryLabels[index] == 'Y'){
            yes += 1;
        }
        min += 1;
        if(min == 60){
            hr += 1;
            min = 0;
        }
    }

    float AHI;

    if((min % 60) != 0)  hr += 1;

    if(hr == 0) AHI = 0;
    else   AHI = (float)yes / (float)hr;

   
    AHI = AHI * 10;

    
    DisplayAI(AHI);

	return 0;
}

