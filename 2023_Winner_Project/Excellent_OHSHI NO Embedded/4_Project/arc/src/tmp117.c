#include "tmp117.h"

#define USE_SS_IIC_X USE_SS_IIC_1

uint8_t Tmp117_IC_BUF[2] = {0};



void Tmp117_Init      ( void ){
	Tmp117_WriteData(0x01, 0x02, 0x20);//    Config   =0000 0010 0010 0000 /0000 0010 0010 000X
								//Normal 			 "0x0220"
								//Extreme			 "0x02A0"
								//High temperature	 "0x0320"
	Tmp117_WriteData(0x07, 0x05, 0xB0);//Temp     Offset 
								//{0x07, 0x00, 0x00},//Temp     Offset 
								/*{0x03, 0x12, 0x80},//Temp Low  Limit  :  Set 10 Celcius
								{0x02, 0x51, 0x20},//Temp High Limit  :  Set 40 Celcius*/
}
void Tmp117_GetTempC  ( uint32_t *Temp                                         ){
	uint16_t Elem;

	Tmp117_ReadData(0x00, 2, &Tmp117_IC_BUF[0]);//MyTMP117_TempReg
	Elem = ((Tmp117_IC_BUF[0] << 8) | Tmp117_IC_BUF[1]);
	*Temp = Elem * 0.0625;
}
void Tmp117_WriteData (  uint8_t   addr, uint8_t Hdata, uint8_t          Ldata ){
    uint8_t WriteData[3];

    WriteData[0] = addr;
    WriteData[1] = Hdata;
    WriteData[2] = Ldata;
    hx_drv_i2cm_write_data(USE_SS_IIC_X ,TMP117_ADDRESS, &WriteData[0], 0, &WriteData[0], 3); 
}
void Tmp117_ReadData  (  uint8_t   addr, uint8_t   len, uint8_t *Tmp117_IC_BUF ){
    uint8_t WriteData[1];
		
    WriteData[0] = addr;
    hx_drv_i2cm_write_data(USE_SS_IIC_X, TMP117_ADDRESS, &WriteData[0], 0, &WriteData[0], 1); 
    hx_drv_i2cm_read_data (USE_SS_IIC_X, TMP117_ADDRESS, &Tmp117_IC_BUF[0], len);
}