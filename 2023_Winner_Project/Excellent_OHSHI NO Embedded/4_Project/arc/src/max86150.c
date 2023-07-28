#include "max86150.h"

#define USE_SS_IIC_X USE_SS_IIC_0

uint8_t MAX86150_IC_BUF[6] = {0};
void Max86150_Init      ( void ){ 
    Max86150_WriteData(0x0D, 0x04); //System Control = "00000100"  
												//[2]  ●: FIFO Enable
												//[1]   : Shutdown Control
												//[0]   : Reset Control
 //                             R{0x00, 0x00},//Interrupt Status 1
 //                             R{0x01, 0x00},//Interrupt Status 2
    Max86150_WriteData(0x02, 0xC0); //Interrupt Enable 1 = "11000000"  
												//[7]  ●: FIFO Almost Full Flag Enable
												//[6]  ●: New PPG FIFO Data Ready Interrupt enable 	
												//[5]   : Ambient Light Cancellation Overflow Interrupt Enable
												//[4]   : Proximity Interrupt Enable
 //                             {0x03, 0x00}, //Interrupt Enable 2 : 
												//[7]   : VDD Out-of-Range Indicator Enable
												//[2]   : New ECG FIFO Data Ready Interrupt Enable
 //                             {0x04, 0x00}, //FIFO Write Pointer
 //                             {0x05, 0x00}, //Overflow Counter
 //                             {0x06, 0x00}, //FIFO Read Pointer
 //                             {0x07, 0x00}, //FIFO Data Register
	Max86150_WriteData(0x08, 0x1F); //FIFO Configuration ? = "00011111"   ☆☆☆☆☆☆
												//[6]   : FIFO Almost Full Interrupt Options
												//[5]   : FIFO Almost Full Flag Options
												//[4]  ●: FIFO Rolls on Full Options
												//[3:0]●: FIFO Almost Full Value
	Max86150_WriteData(0x09, 0x12); //FIFO Data Control Register 1 
												//[7:4]●: FD2=PPGIR	
												//[3:0]●: FD1=PPGRED
	Max86150_WriteData(0x0A, 0x00); //FIFO Data Control Register 2
												//[7:4] : FD4=None	
												//[3:0] : FD3=None					
	Max86150_WriteData(0x0E, 0x16); //PPG Configuration 1 = "[00 01][01 10]"   
												//[7:6] : SpO2 ADC Range Contro
													//PPG_ADC_RGE<1:0>    LSB [pA]    FULL SCALE [nA]
													//   ●00           7.8125               4096
													//    01           15.625               8192
													//    10            31.25              16384
													//    11             62.5              32768
												//[5:2] : SpO2 Sample Rate Control
													//PPG_SR<3:0>    SAMPLES PER SECOND PULSES PER SAMPLE,    N
													//	0100                          100                     1
													// ●0101                          200 					  1
													//  0110                          400                     1
													//  0111                          800                     1
													//  1000                         1000                     1
													//  1001                         1600                     1
													//  1010                         3200                     1
												//[1:0] : LED Pulse Width Control
													//PPG_LED_PW<1:0>    PULSE WIDTH [µs]    RES BITS
													//    00                      50            19
													//   01                     100            19
													//   ●10                     200            19
													//    11                     400            19
												//Maximum sample rates  :  NUMBER OF ADC CONVERSIONS
												//PPG_LED_PW= 0(50µs)    1(100µs)    2(200µs)    3(400µs)                              
												//2 LED, N=1 	1600 		800 		800 		400
	Max86150_WriteData(0x0F, 0x00); //PPG Configuration 2   	
													//[2:0] : Sample Averaging Options 
 //                             {0x10, 0x00}, //Prox Interrupt Threshold
	Max86150_WriteData(0x11, 0x19); //LED1 PA  PPGIR    I = 10.0mA                 
	Max86150_WriteData(0x12, 0x19); //LED1 PA  PPGRED   I = 10.0mA        
	Max86150_WriteData(0x14, 0x05); //LED Range         i < 100mA             
//                              {0x15, 0x00}, //LED PILOT PA
//                              {0x3C, 0x00}, //ECG Configuration 1   
//                              {0x3E, 0x00}, //ECG Configuration 3   
//                              R{0xFF, 0x00},//Part ID                             
}
void Max86150_GetPPG    ( uint16_t      N, uint32_t *DataIR, uint32_t         *DataRED){
    uint16_t count = 0;//2^16 = 65536 > MAX = 10000
	uint32_t Elem[2];//PPGIR PPGRED
    while(1){
		Max86150_ReadData(0x00, 1, &MAX86150_IC_BUF[0]);
		if ((MAX86150_IC_BUF[0]&0x40)>0){
			Max86150_ReadData(0x07, 6, &MAX86150_IC_BUF[0]);
			Elem[0] = ((MAX86150_IC_BUF[0]<<16) |	(MAX86150_IC_BUF[1]<<8) | MAX86150_IC_BUF[2])&0x07ffff;//PPGRED
			Elem[1] = ((MAX86150_IC_BUF[3]<<16) |	(MAX86150_IC_BUF[4]<<8) | MAX86150_IC_BUF[5])&0x07ffff;//PPGIR
			DataIR [count] =  Elem[1];
			DataRED[count] =  Elem[0];
			count = count + 1;
		}
        if(count == N){
            break;
        }
	}	
}
void Max86150_WriteData (  uint8_t   Addr,  uint8_t    Data                           ){
    uint8_t WriteData[2];

    WriteData[0] = Addr;
    WriteData[1] = Data;
    hx_drv_i2cm_write_data(USE_SS_IIC_X ,MAX86150_ADDRESS, &WriteData[0], 0, &WriteData[0], 2); 
}
void Max86150_ReadData  (  uint8_t   Addr,  uint8_t     Len, uint8_t  *MAX86150_IC_BUF){
    uint8_t WriteData[1];
		
    WriteData[0] = Addr;
    hx_drv_i2cm_write_data(USE_SS_IIC_X, MAX86150_ADDRESS, &WriteData[0], 0, &WriteData[0], 1); 
    hx_drv_i2cm_read_data (USE_SS_IIC_X, MAX86150_ADDRESS, &MAX86150_IC_BUF[0], Len);
}