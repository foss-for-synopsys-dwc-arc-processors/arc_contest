#include "pca9672.h"

#define USE_SS_IIC_X USE_SS_IIC_0

 uint8_t Pca9672_IC_BUF[1] = {0};
    uint8_t benc = 0;
void Pca9672_Init(void){
    Pca9672_WriteData(0x00, 0x06);//Software Reset
    Pca9672_WriteData(  28, 0x00);
    board_delay_ms(10);
}

void Pca9672_DcPin(uint8_t Pin){
    uint8_t WriteData[1]={0};

    while (1){
	    Pca9672_ReadData(0x00, 1, &Pca9672_IC_BUF[0]);
        if(Pca9672_IC_BUF[0]==28||Pca9672_IC_BUF[0]==24||Pca9672_IC_BUF[0]==12||Pca9672_IC_BUF[0]==8)
            break;
    }
    if(Pin==0){//DC = LOW = 0 = CMD        
        if(Pca9672_IC_BUF[0]== 28 || Pca9672_IC_BUF[0]==12 ){
            WriteData[0] = Pca9672_IC_BUF[0] - 4;
//            printf("DC=0 = %d to %d\n", Pca9672_IC_BUF[0], WriteData[0]);
            hx_drv_i2cm_write_data(USE_SS_IIC_X, PCA9672_ADDRESS, &WriteData[0], 0, &WriteData[0], 1);
        }
//        else
//            printf("DC=0 = %d to %d\n", Pca9672_IC_BUF[0], WriteData[0]);
    }
    else if(Pin==1){//DC = HIGH = 1 = DATA   
        if(Pca9672_IC_BUF[0]== 24 || Pca9672_IC_BUF[0]== 8  ){
            WriteData[0] = Pca9672_IC_BUF[0] + 4;
//            printf("DC=1 = %d to %d\n", Pca9672_IC_BUF[0], WriteData[0]);
            hx_drv_i2cm_write_data(USE_SS_IIC_X, PCA9672_ADDRESS, &WriteData[0], 0, &WriteData[0], 1);
        }
//        else
//            printf("DC=1 = %d to %d\n", Pca9672_IC_BUF[0], WriteData[0]);
    }
}
void Pca9672_GetState(uint8_t *Bhl){
	Pca9672_ReadData(0x00, 1, &Pca9672_IC_BUF[0]);
	*Bhl = Pca9672_IC_BUF[0];
}
void Pca9672_WriteData(uint8_t addr, uint8_t data){
    uint8_t WriteData[2];

    WriteData[0] = addr;
    WriteData[1] = data;
    if(WriteData[1] == 0x00)
        hx_drv_i2cm_write_data(USE_SS_IIC_X, PCA9672_ADDRESS, &WriteData[0], 0, &WriteData[0], 1); 
    else
        hx_drv_i2cm_write_data(USE_SS_IIC_X, PCA9672_ADDRESS, &WriteData[0], 0, &WriteData[0], 2); 
}
void Pca9672_ReadData (uint8_t addr, uint8_t len, uint8_t *Pca9672_IC_BUF){
    uint8_t WriteData[1];
		
    WriteData[0] = addr;
//    hx_drv_i2cm_write_data(USE_SS_IIC_X, PCA9672_ADDRESS, &WriteData[0], 0, &WriteData[0], 1); 
    hx_drv_i2cm_read_data (USE_SS_IIC_X, PCA9672_ADDRESS, &Pca9672_IC_BUF[0], len);
}