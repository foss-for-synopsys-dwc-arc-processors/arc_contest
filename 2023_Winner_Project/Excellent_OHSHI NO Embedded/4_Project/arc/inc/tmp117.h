#ifndef _IC2_TMP117_H_
#define _IC2_TMP117_H_

#include "hx_drv_iic_m.h"
#include "arc_timer.h"

#define TMP117_ADDRESS	 0x48

void Tmp117_Init      ( void ); 	
void Tmp117_GetTempC  ( uint32_t *Temp                                         );
void Tmp117_WriteData (  uint8_t   addr, uint8_t Hdata, uint8_t          Ldata );
void Tmp117_ReadData  (  uint8_t   addr, uint8_t   len, uint8_t *Tmp117_IC_BUF );
#endif