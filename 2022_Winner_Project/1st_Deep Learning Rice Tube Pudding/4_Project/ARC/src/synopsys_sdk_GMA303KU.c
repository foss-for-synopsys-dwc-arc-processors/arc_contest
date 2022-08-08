#include "synopsys_sdk_GMA303KU.h"

#define USE_SS_IIC_X USE_SS_IIC_0

uint8_t GMA303KU_Init(void)
{
    uint8_t read_data[1];

    I2C_GMA303KU_READ_API(0x00, &read_data[0], 1);

    I2C_GMA303KU_WRITE_API(0x18, 0x40);
    I2C_GMA303KU_WRITE_API(0x15, 0x2A);
    I2C_GMA303KU_WRITE_API(0x16, 0x09);

    return read_data[0];
}

uint16_t GMA303KU_Get_Data(int16_t * x, int16_t * y, int16_t * z, int16_t * t)
{
    uint16_t reg_04_data;
    uint8_t read_data[11];

    I2C_GMA303KU_READ_API(0x04, &read_data[0], 11);   

    reg_04_data = ((uint16_t) read_data[1] << 8) | read_data[0];

    *x = ((uint16_t) read_data[4] << 8) | read_data[3];
    *y = ((uint16_t) read_data[6] << 8) | read_data[5];
    *z = ((uint16_t) read_data[8] << 8) | read_data[7];
    *t = ((uint16_t) read_data[10] << 8) | read_data[9];

    return reg_04_data;
}

void I2C_GMA303KU_WRITE_API(uint8_t reg_addr, uint8_t reg_data)
{
    uint8_t write_data[2];

    write_data[0] = reg_addr;
    write_data[1] = reg_data;   
    hx_drv_i2cm_write_data(USE_SS_IIC_X, GMA303KU_ADDRESS, write_data, 0, write_data, 2); 
}

void I2C_GMA303KU_READ_API(uint8_t reg_addr, uint8_t * read_buf, uint8_t read_len)
{
    uint8_t write_data[1];

    write_data[0] = reg_addr;
    hx_drv_i2cm_write_data(USE_SS_IIC_X, GMA303KU_ADDRESS, write_data, 0, write_data, 1); 
    hx_drv_i2cm_read_data(USE_SS_IIC_X, GMA303KU_ADDRESS, read_buf, read_len);
}
