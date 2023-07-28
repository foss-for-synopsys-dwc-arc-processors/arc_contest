#ifndef _SPI_ST7735_H_
#define _SPI_ST7735_H_
#include "board_config.h"
#include "hx_drv_spi_m.h"
#include "SC16IS750_Bluepacket.h"
#include "pca9672.h"

#define SWRESET 0x01
#define SLPOUT 0x11
#define FRMCTR1 0xB1
#define FRMCTR2 0xB2
#define FRMCTR3 0xB3
#define INVCTR 0xB4
#define PWCTR1 0xC0
#define PWCTR2 0xC1
#define PWCTR3 0xC2
#define PWCTR4 0xC3
#define PWCTR5 0xC4
#define VMCTR1 0xC5
#define MADCTL 0x36
#define GMCTRP1 0xE0
#define GMCTRN1 0xE1
#define COLMOD 0x3A
#define DISPON 0x29 
#define CASET 0x2A
#define RASET 0x2B
#define RAMWR 0x2C

void St7735_Init(void); 	
void St7735_Display(uint8_t Level, uint8_t Breathe,uint8_t Heart, uint32_t BodyTemp, int Oxygen, int PressureUP, int PressureDOWN);
void St7735_WriteCommand(uint8_t Command);
void St7735_WriteData   (uint8_t Data);
void St7735_WriteColor  (uint8_t ColorH, uint8_t ColorL);
#endif