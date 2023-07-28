#include "sd178b.h"

#define USE_SS_IIC_X USE_SS_IIC_1

uint8_t SD178B_IC_BUF[6] = {0};
void Sd178b_Init      ( void ){ 
    uint8_t   WriteData[22]={   0x86 ,0xFF};//聲量調整
    hx_drv_i2cm_write_data(USE_SS_IIC_X ,SD178B_ADDRESS, &WriteData[0], 0, &WriteData[0], 2); 
}
void Sd178b_Call      ( uint8_t Call                                                ){
    uint16_t count = 0;//2^16 = 65536 > MAX = 10000
	uint32_t Elem[2];//PPGIR PPGRED
    //uint8_t WriteData[16]={0xB5 ,0xBE ,0xAD ,0xB5 ,0xAC ,0xEC ,0xA7 ,0xDE ,0x31 ,0x37 ,0x38 ,0x42 ,0xB4 ,0xFA ,0xB8 ,0xD5};
    if(Call==0){
    uint8_t   WriteData[20]={  0xAB ,0xF6 ,0xA4 ,0x55 ,0xAB ,0xF6 ,0xB6 ,0x73 ,0xB6 ,0x7D ,0xA9 ,0x6C ,0xC0 ,0xCB ,0xB6 ,0xCB,0xA4 ,0xC0 ,0xC3 ,0xFE};
    hx_drv_i2cm_write_data(USE_SS_IIC_X ,SD178B_ADDRESS, &WriteData[0], 0, &WriteData[0], 20); 
    }
    if(Call==1){
        
    uint8_t   WriteData[22]={  0xA4,0xDF,0xB8,0xF5,  0xAF,0xDF,0xB7,0x76,   0xB2,0xA7,0xB1,0x60,0xBD,0xD0,0xBA,0xC9,0xB3,0x74,0xB4,0x4E,0xC2,0xE5};
    hx_drv_i2cm_write_data(USE_SS_IIC_X ,SD178B_ADDRESS, &WriteData[0], 0, &WriteData[0], 22); 
    }
}
void Sd178b_WriteData ( uint8_t   addr, uint8_t Hdata, uint8_t          Ldata ){
    uint8_t WriteData[2];

    WriteData[0] = Hdata;
    WriteData[1] = Ldata;
    hx_drv_i2cm_write_data(USE_SS_IIC_X ,SD178B_ADDRESS, &WriteData[0], 0, &WriteData[0], 2);  
}