#ifndef _IC2_PCA9672_H_
#define _IC2_PCA9672_H_

#include "hx_drv_iic_m.h"
#include "board_config.h"
#define PCA9672_ADDRESS	 0x20

void Pca9672_Init(void); 	
void Pca9672_DcPin(uint8_t Pin);
void Pca9672_GetState(uint8_t *Bhl);
void Pca9672_WriteData(uint8_t addr, uint8_t data);
void Pca9672_ReadData (uint8_t addr, uint8_t len, uint8_t *Pca9672_Buffer);
#endif