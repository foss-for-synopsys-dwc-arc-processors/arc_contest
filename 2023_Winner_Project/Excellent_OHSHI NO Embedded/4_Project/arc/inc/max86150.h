#ifndef _IC2_MAX86150_H_
#define _IC2_MAX86150_H_

#include "hx_drv_iic_m.h"

#define MAX86150_ADDRESS	 0x5E

void Max86150_Init      ( void ); 	
void Max86150_GetPPG    ( uint16_t      N, uint32_t *DataIR, uint32_t         *DataRED);
void Max86150_WriteData (  uint8_t   Addr,  uint8_t    Data                           );
void Max86150_ReadData  (  uint8_t   Addr,  uint8_t     Len, uint8_t  *MAX86150_IC_BUF);

#endif