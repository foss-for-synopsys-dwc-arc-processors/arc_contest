#include "max86150.h"
#include "math.h"
#include "stdio.h"
#include "string.h"
#include "synopsys_sdk_GMA303KU.h"
#define uart_buf_size 100
#define limits -290

char uart_buf[uart_buf_size] = {0};
uint8_t Tobin_buf[8] = {0};
uint8_t buffer[MAX][bytes] = {0};
uint8_t read_buf[20] = {0};
DEV_IIC * iic1_ptr;
DEV_UART * uart0_ptr;
uint32_t count, read = 0;
int16_t x;
int16_t y;
int16_t z;
int16_t t;

void InitUART(void){
    uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);
    sprintf(uart_buf, "I2C teraterm.log\r\n\n");    
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
}

void InitI2C(void){
    iic1_ptr = hx_drv_i2cm_get_dev(USE_SS_IIC_X);
    iic1_ptr->iic_open(DEV_MASTER_MODE, IIC_SPEED_FAST); 

}

void InitMax86150(void){
    InitI2C();
    uint8_t initial_data_write[8][2] = { 
                                         {0x02, 0x80}, //Interrupt Enable 1
                                         {0x03, 0x04}, //Interrupt Enable 2
                                         {0x08, 0x10}, //FIFO Configuration 
										 {0x09, 0x09}, //FIFO Data Control Register 1
										 {0x0A, 0x00}, //FIFO Data Control Register 2
										 {0x3C, 0x03}, //ECG Configuration 1   //0x02   0x03
										 {0x3E, 0x00}, //ECG Configuration 3    //0x0D  0x00
                                         {0x0D, 0x04}, //System Control;
                                        }; 
    for(int i = 0; i < 8; i++){
        Max86150_WriteData(initial_data_write[i][0], initial_data_write[i][1]);
    }
    board_delay_ms(500);
    RestRead();
}


// {0xFF, 0x54},
// {0xFF, 0x4D},
// {0xCE, 0x0A},
// {0xCF, 0x18},
// {0xFF, 0x00},

void Max86150_ReadData(uint8_t addr, uint8_t len, uint8_t * read_buf){
    uint8_t WriteData[2];
    WriteData[0] = addr;

    hx_drv_i2cm_write_data(USE_SS_IIC_X, MAX86150_Address, &WriteData[0], 0, &WriteData[0], 1); 
    hx_drv_i2cm_read_data(USE_SS_IIC_X, MAX86150_Address, &read_buf[0], len);
}

void Max86150_WriteData(uint8_t addr, uint8_t data){
    uint8_t WriteData[2];

    WriteData[0] = addr;
    WriteData[1] = data;
    hx_drv_i2cm_write_data(USE_SS_IIC_X, MAX86150_Address, &WriteData[0], 0, &WriteData[0], 2); 
}

void uint8_tTobin_1Byte(uint8_t data){
    uint8_t buf;
    for(int i = 7; i >= 0; i--)
    {
        buf = 128;
        for(int j = 7; j > i; j--){
            buf = buf / 2;
        }
        Tobin_buf[i] = (data & buf) >> i;
    }
        
}

void DisplayData(char base, int data, char Text[], uint8_t WordDisplay){
    switch (WordDisplay)
    {
    case 0:{
        switch (base)
        {
        case 'B':{
            uint8_tTobin_1Byte(data);
            sprintf(uart_buf, "%d%d%d%d %d%d%d%d\r\n",  Tobin_buf[7], Tobin_buf[6], Tobin_buf[5], Tobin_buf[4],
                                                        Tobin_buf[3], Tobin_buf[2], Tobin_buf[1], Tobin_buf[0]);
            break;
        }
        case 'D':{
            sprintf(uart_buf, "%d\r\n", data);
            break;
        }
        case 'H':{
            sprintf(uart_buf, "%x\r\n", data);
            break;
        }  
        default:
            sprintf(uart_buf, "base: Bin or Dec or Hex\r\n");
            break;
        }
        break;
    }

    case 1:{
        switch (base)
        {
        case 'B':{
            uint8_tTobin_1Byte(data);
            sprintf(uart_buf, "%s：%d%d%d%d %d%d%d%d\r\n", Text, Tobin_buf[7], Tobin_buf[6], Tobin_buf[5], Tobin_buf[4],
                                                         Tobin_buf[3], Tobin_buf[2], Tobin_buf[1], Tobin_buf[0]);
            break;
        }
        case 'D':{
            sprintf(uart_buf, "%s：%d\r\n", Text, data);
            break;
        }
        case 'H':{
            sprintf(uart_buf, "%s：%x\r\n", Text, data);
            break;
        }  
        default:
            sprintf(uart_buf, "base: Bin or Dec or Hex\r\n");
            break;
        }
        break;
    }
    default:
        sprintf(uart_buf, "WordDisplay: 0 or 1\r\n");
        break;
    }
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));

}

void GetECG(float * data){
    uint8_t write_point;

    Max86150_ReadData(0x04, 1, &read_buf[0]);
    write_point = read_buf[0];

    int NumAvailableSamples = write_point - read;
   
    if(NumAvailableSamples < 0){
        NumAvailableSamples += 32;
    }

    if(NumAvailableSamples == 0){
        char test[10] = "NO NO NO"; 
        uart0_ptr->uart_write(test, strlen(test));
    }

    for(int i = 0; i < NumAvailableSamples; i++){
            Max86150_ReadData(0x07, 3, &read_buf[0]);
 
            int buf = ((read_buf[0] << 16) | (read_buf[1] << 8) | read_buf[2]) & 0x3ffff;
          
            if(buf > 0x20000){
                buf -= 0x20000;
                buf = -1 * (0x1ffff - buf + 1);
            }

            data[count] = buf;

            count += 1;
            if(count == MAX)
            {
                break;
            }
    }
    read = write_point;
}

int GetECGloop(float * data){
    
    count = 0;
    while(1){
        board_delay_ms(10);
        uint16_t reg_04_data = GMA303KU_Get_Data(&x, &y, &z, &t);
        if(z > limits ){
            break;
        }
        GetECG(data);
        if(count == MAX)
        {
            break;
        }
    }
    return count;
}

void ReadRegister(void){
    uint8_t Interrupt1, Interrupt2, write_point, Overflow, read_point;
    Max86150_ReadData(0x00, 1, &read_buf[0]);
    Interrupt1 = read_buf[0];
    Max86150_ReadData(0x01, 1, &read_buf[0]);
    Interrupt2 = read_buf[0];
    Max86150_ReadData(0x04, 1, &read_buf[0]);
    write_point = read_buf[0];
    Max86150_ReadData(0x05, 1, &read_buf[0]);
    Overflow = read_buf[0];
    Max86150_ReadData(0x06, 1, &read_buf[0]);
    read_point = read_buf[0];

    buffer[count][0] = Interrupt1;
    buffer[count][1] = Interrupt2;
    buffer[count][2] = write_point;
    buffer[count][3] = Overflow;
    buffer[count][4] = read_point;

    
    if(count % 2 == 0){
        Max86150_ReadData(0x07, 3, &read_buf[0]);
    }
    count += 1;
}

void RestRead(void){
    uint8_t Interrupt1, Interrupt2, write_point, OverFlow, read_point;
    Max86150_ReadData(0x00, 1, &read_buf[0]);
    while (1)
    {
        Max86150_ReadData(0x07, 3, &read_buf[0]);
        Max86150_ReadData(0x05, 1, &read_buf[0]);
        OverFlow = read_buf[0];
        if(OverFlow != 0x1f){
            for (int i = 0; i < 31; i++)
            {
                Max86150_ReadData(0x07, 3, &read_buf[0]);
            }
            break;
        }
    }
}



void DisplayDataFloat(float input){
    uint8_t sign;
    int integer;
    uint8_t decimal[6];
    if(input < 0){
        sign = '-';
        input = -1 * input;
        integer = input;
    }
    else{
        sign = '+';
        integer = input;
    }
    for(int i = 1; i <= 6; i++){
        int buf = input * (int)pow(10, i);
        decimal[i - 1] = buf % 10;
    }
    sprintf(uart_buf, "%c%d.%d%d%d%d%d%d\r\n", sign, integer, decimal[0], decimal[1],
                                               decimal[2], decimal[3], decimal[4], decimal[5]);

    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
}