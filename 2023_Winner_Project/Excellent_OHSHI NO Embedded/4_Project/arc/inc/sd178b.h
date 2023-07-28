#ifndef _IC2_SD178B_H_
#define _IC2_SD178B_H_

#include "hx_drv_iic_m.h"
#include "arc_timer.h"

#define SD178B_ADDRESS	 0x20

void Sd178b_Init      ( void ); 	
void Sd178b_Call      ( uint8_t Call                                            );
void Sd178b_WriteData ( uint8_t   addr, uint8_t Hdata, uint8_t          Ldata );
#endif