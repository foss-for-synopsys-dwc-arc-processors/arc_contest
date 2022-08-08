#ifndef _I2C_MAX86150_H_
#define _I2C_MAX86150_H_



#include "hx_drv_uart.h"
#include "hx_drv_iic_m.h"

#define MAX86150_Address 0x5E

#define extra 0
#define MAX 12000 + extra
#define bytes 3
#define USE_SS_IIC_X USE_SS_IIC_1

void InitUART(void);
void InitI2C(void);
void InitMax86150(void);
void Max86150_ReadData(uint8_t addr, uint8_t len, uint8_t * read_buf);
void Max86150_WriteData(uint8_t addr, uint8_t data);
void uint8_tTobin_1Byte(uint8_t data);
void DisplayData(char base, int data, char Text[], uint8_t WordDisplay);
void GetECG(float * data);
int GetECGloop(float * data);
void ReadRegister(void);
void RestRead(void);
void DisplayDataFloat(float input);
#endif