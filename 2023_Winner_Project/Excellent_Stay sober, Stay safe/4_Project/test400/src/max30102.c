#include "max30102.h"
#include <stdio.h>
#include "hx_drv_iic_m.h"
#include "hx_drv_uart.h"
#include "string.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "board_config.h"
#include "arc_timer.h"
#include "hx_drv_spi_s.h"
#include "spi_slave_protocol.h"
#include "hardware_config.h"
#include "hx_drv_uart.h"
#include "board.h"
#define FIFO_RD_ADDRESS 0x06
#define MODE_CONFIG_ADDRESS 0x09
#define MAX30102_I2C_ADDRESS 0x57

uint8_t MODE1_ADDRESS = 0x06;
uint8_t ADC_CONFIG_ADDRESS = 0x0A;
uint8_t LED_bright_CONFIG_ADDRESS = 0x0D;
uint8_t FIFO_DATA_ADDRESS = 0x07;
uint8_t FIFO_WR_ADDRESS = 0x04;

static DEV_IIC *iic;


static DEV_IIC *iic;
DEV_UART *uart0_ptr;
char uart_buf[100] = {0};

static uint8_t readRegister1(uint8_t address);
static void writeRegister1(uint8_t address, uint8_t data);

static uint8_t readRegister2(uint8_t address);
static void writeRegister2(uint8_t address, uint8_t data);


void InitUART(void) {
    uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);
    uart0_ptr->uart_open(UART_BAUDRATE_115200);
    sprintf(uart_buf, "I2C teraterm.log\r\n\n");
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
}

uint8_t readRegister1(uint8_t address) {
    uint8_t data;
    hx_drv_i2cm_write_data(USE_SS_IIC_0, MAX30102_I2C_ADDRESS, &address, 1, NULL, 0);
    hx_drv_i2cm_read_data(USE_SS_IIC_0, MAX30102_I2C_ADDRESS, &data, 1);
    return data;
}

void writeRegister1(uint8_t address, uint8_t data) {
    uint8_t write_buf[2] = {address, data};
    hx_drv_i2cm_write_data(USE_SS_IIC_0, MAX30102_I2C_ADDRESS, write_buf, 0, write_buf, 2);
}

uint8_t readRegister2(uint8_t address) {
    uint8_t data;
    hx_drv_i2cm_write_data(USE_SS_IIC_1, MAX30102_I2C_ADDRESS, &address, 1, NULL, 0);
    hx_drv_i2cm_read_data(USE_SS_IIC_1, MAX30102_I2C_ADDRESS, &data, 1);
    return data;
}

void writeRegister2(uint8_t address, uint8_t data) {
    uint8_t write_buf[2] = {address, data};
    hx_drv_i2cm_write_data(USE_SS_IIC_1, MAX30102_I2C_ADDRESS, write_buf, 0, write_buf, 2);
}

void max30102_1setup() {
    iic = hx_drv_i2cm_get_dev(USE_SS_IIC_0);
    iic->iic_open(DEV_MASTER_MODE, IIC_SPEED_STANDARD);
    writeRegister1(MODE_CONFIG_ADDRESS, 0x02);//2-led mode(0x03) 1-led mode(0x02)
    writeRegister1(ADC_CONFIG_ADDRESS, 0x68);//sample rate + pulse width
    writeRegister1(0x08,0x10 );//set register roll-over
    writeRegister1(0X0C, 0xc0);//2f);//brightness  //soso
    writeRegister1(0X0D, 0xc0);//2f);//brightness
    printf("setup1 done\r\n");
   
}

void max30102_2setup() {
    iic = hx_drv_i2cm_get_dev(USE_SS_IIC_1);
    iic->iic_open(DEV_MASTER_MODE, IIC_SPEED_STANDARD);
    writeRegister2(MODE_CONFIG_ADDRESS, 0x02);//2-led mode(0x03) 1-led mode(0x02)
    writeRegister2(ADC_CONFIG_ADDRESS, 0x68);//sample rate + pulse width
    writeRegister2(0x08,0x10 );//set register roll-over
    writeRegister2(0X0C, 0xc0);//2f);//brightness //soso
    writeRegister2(0X0D, 0xc0);//2f);//brightness
    printf("setup2 done\r\n");
   
}

//----------------I2C master0 兩顆LED---------------------
void max30102_1loop2() {
    int rd_ptr = readRegister1(FIFO_RD_ADDRESS);
    int wr_ptr = readRegister1(0x04);
    

    if (wr_ptr == rd_ptr) {
        return; // No new data
    }
    

    int num_samples = wr_ptr - rd_ptr;
    if (num_samples < 0) {
        num_samples += 32;
    }

    uint8_t buffer[6*num_samples];
    uint8_t fifo_data_address = 0x07;
    hx_drv_i2cm_write_data(USE_SS_IIC_0, MAX30102_I2C_ADDRESS, &fifo_data_address, 1, NULL, 0);
    hx_drv_i2cm_read_data(USE_SS_IIC_0, MAX30102_I2C_ADDRESS, buffer, 6*num_samples);

    for (int i=0; i<num_samples; i++) {
        int idx = i*6;
        uint32_t red = (buffer[idx] << 16) | (buffer[idx+1] << 8) | buffer[idx+2];
        uint32_t ir = (buffer[idx+3] << 16) | (buffer[idx+4] << 8) | buffer[idx+5];
        printf("red = %d, ir = %d\n", ir, red);
    }

    rd_ptr += num_samples;
    if (rd_ptr >= 32) {
        rd_ptr -= 32;
    }

    writeRegister1(FIFO_RD_ADDRESS, rd_ptr);
    //writeRegister(MODE_CONFIG_ADDRESS, 0x03);
}

//-------------------I2C master0 單顆LED---------------------
int max30102_1loop1(uint32_t *red_values) {
    int rd_ptr = readRegister1(FIFO_RD_ADDRESS);
    int wr_ptr = readRegister1(0x04);

    if (wr_ptr == rd_ptr) {
        return 0; // No new data
    }

    int num_samples = wr_ptr - rd_ptr;
    if (num_samples < 0) {
        num_samples += 32;
    }

    uint8_t buffer[3 * num_samples];
    uint8_t fifo_data_address = 0x07;
    hx_drv_i2cm_write_data(USE_SS_IIC_0, MAX30102_I2C_ADDRESS, &fifo_data_address, 1, NULL, 0);
    hx_drv_i2cm_read_data(USE_SS_IIC_0, MAX30102_I2C_ADDRESS, buffer, 3 * num_samples);

    for (int i = 0; i < num_samples; i++) {
        int idx = i * 3;
        red_values[i] = (buffer[idx] << 16) | (buffer[idx + 1] << 8) | buffer[idx + 2];
    }

    rd_ptr += num_samples;
    if (rd_ptr >= 32) {
        rd_ptr -= 32;
    }

    writeRegister1(FIFO_RD_ADDRESS, rd_ptr);

    return num_samples;
}

//-------------------I2C master1 單顆LED---------------------
int max30102_2loop1(uint32_t *red_values) {
    int rd_ptr = readRegister2(FIFO_RD_ADDRESS);
    int wr_ptr = readRegister2(0x04);

    if (wr_ptr == rd_ptr) {
        return 0; // No new data
    }

    int num_samples = wr_ptr - rd_ptr;
    if (num_samples < 0) {
        num_samples += 32;
    }

    uint8_t buffer[3 * num_samples];
    uint8_t fifo_data_address = 0x07;
    hx_drv_i2cm_write_data(USE_SS_IIC_1, MAX30102_I2C_ADDRESS, &fifo_data_address, 1, NULL, 0);
    hx_drv_i2cm_read_data(USE_SS_IIC_1, MAX30102_I2C_ADDRESS, buffer, 3 * num_samples);

    for (int i = 0; i < num_samples; i++) {
        int idx = i * 3;
        red_values[i] = (buffer[idx] << 16) | (buffer[idx + 1] << 8) | buffer[idx + 2];
    }

    rd_ptr += num_samples;
    if (rd_ptr >= 32) {
        rd_ptr -= 32;
    }

    writeRegister2(FIFO_RD_ADDRESS, rd_ptr);

    return num_samples;
}






