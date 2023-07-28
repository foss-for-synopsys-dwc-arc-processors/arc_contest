#ifndef _SC16IS750_H_

#define _SC16IS750_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "embARC.h"
#include "embARC_debug.h"
#include "board_config.h"
#include "arc_timer.h"
#include "hardware_config.h"

#include "hx_drv_iic_m.h"
#include "hx_drv_iomux.h"
#include "hx_drv_spi_m.h"
#include "dw_spi.h"
#include "dw_spi_hal.h"

//Device Address

int16_t hal_spim_write(uint8_t cs_number, uint8_t *data, uint16_t data_len);

int16_t i2cm_write_reg(uint8_t isChA,uint8_t reg_addr, uint8_t *data, uint16_t data_len);
uint8_t i2cm_read_reg(uint8_t isChA, uint8_t reg_addr);
uint8_t i2cm_read_stream_reg(uint8_t isChA, uint8_t reg_addr, uint8_t *data_buf, uint16_t data_len);


int16_t spim_write_reg(uint8_t isChA, uint8_t reg_addr, uint8_t *data, uint16_t data_len);
uint8_t spim_read_reg(uint8_t isChA, uint8_t reg_addr);
uint8_t spim_read_stream_reg(uint8_t isChA, uint8_t reg_addr, uint8_t *data_buf, uint16_t data_len);

void HX_GPIOSetup();
void IRQSetup();
int16_t SetBaudrate(uint8_t interface, uint8_t isChA, uint32_t baudrate);
void SetLine(uint8_t interface, uint8_t isChA, uint8_t data_length, uint8_t parity_select, uint8_t stop_length );
void FIFOEnable(uint8_t interface, uint8_t isChA, uint8_t fifo_enable);
void FIFOSetTriggerLevel(uint8_t interface, uint8_t rx_fifo, uint8_t length);

void GPIOSetPinMode(uint8_t interface, uint8_t isChA, uint8_t pin_number, uint8_t i_o);
void GPIOSetPinState(uint8_t interface, uint8_t isChA, uint8_t pin_number, uint8_t pin_state);
uint8_t GPIOGetPinState(uint8_t interface, uint8_t isChA, uint8_t pin_number);
void InitGPIOSetup(uint8_t interface);
void TestGPIO(uint8_t interface);

void IRQ_State(uint8_t interface, uint8_t isChA);
void InterruptControl(uint8_t interface, uint8_t isChA, uint8_t int_ena);
void SetPinInterrupt(uint8_t interface, uint8_t isChA, uint8_t pin_number);

int16_t UARTTest(uint8_t interface);
int16_t UartInit(uint8_t interface);

//A:VDD
//B:GND
//C:SCL
//D:SDA
#define     SC16IS750_ADDRESS_AA     (0X90)
#define     SC16IS750_ADDRESS_AB     (0X92)
#define     SC16IS750_ADDRESS_AC     (0X94)
#define     SC16IS750_ADDRESS_AD     (0X96)
#define     SC16IS750_ADDRESS_BA     (0X98)
#define     SC16IS750_ADDRESS_BB     (0X9A)
#define     SC16IS750_ADDRESS_BC     (0X9C)
#define     SC16IS750_ADDRESS_BD     (0X9E)
#define     SC16IS750_ADDRESS_CA     (0XA0)
#define     SC16IS750_ADDRESS_CB     (0XA2)
#define     SC16IS750_ADDRESS_CC     (0XA4)
#define     SC16IS750_ADDRESS_CD     (0XA6)
#define     SC16IS750_ADDRESS_DA     (0XA8)
#define     SC16IS750_ADDRESS_DB     (0XAA)
#define     SC16IS750_ADDRESS_DC     (0XAC)
#define     SC16IS750_ADDRESS_DD     (0XAE)


//General Registers
#define     SC16IS750_REG_RHR        (0x00)
#define     SC16IS750_REG_THR        (0X00)
#define     SC16IS750_REG_IER        (0X01)
#define     SC16IS750_REG_FCR        (0X02)
#define     SC16IS750_REG_IIR        (0X02)
#define     SC16IS750_REG_LCR        (0X03)
#define     SC16IS750_REG_MCR        (0X04)
#define     SC16IS750_REG_LSR        (0X05)
#define     SC16IS750_REG_MSR        (0X06)
#define     SC16IS750_REG_SPR        (0X07)
#define     SC16IS750_REG_TCR        (0X06)
#define     SC16IS750_REG_TLR        (0X07)
#define     SC16IS750_REG_TXLVL      (0X08)
#define     SC16IS750_REG_RXLVL      (0X09)
#define     SC16IS750_REG_IODIR      (0X0A)
#define     SC16IS750_REG_IOSTATE    (0X0B)
#define     SC16IS750_REG_IOINTENA   (0X0C)
#define     SC16IS750_REG_IOCONTROL  (0X0E)
#define     SC16IS750_REG_EFCR       (0X0F)

//Special Registers
#define     SC16IS750_REG_DLL        (0x00)
#define     SC16IS750_REG_DLH        (0X01)

//Enhanced Registers
#define     SC16IS750_REG_EFR        (0X02)
#define     SC16IS750_REG_XON1       (0X04)
#define     SC16IS750_REG_XON2       (0X05)
#define     SC16IS750_REG_XOFF1      (0X06)
#define     SC16IS750_REG_XOFF2      (0X07)

//interrupt 
#define     SC16IS750_INT_CTS        (0X80)
#define     SC16IS750_INT_RTS        (0X40)
#define     SC16IS750_INT_XOFF       (0X20)
#define     SC16IS750_INT_SLEEP      (0X10)
#define     SC16IS750_INT_MODEM      (0X08)
#define     SC16IS750_INT_LINE       (0X04)
#define     SC16IS750_INT_THR        (0X02)
#define     SC16IS750_INT_RHR        (0X01)

//Application Related 

#define     SC16IS750_CRYSTCAL_FREQ (29491200UL) 
//#define     SC16IS750_CRYSTCAL_FREQ (14745600UL) 
//#define 	SC16IS750_CRYSTCAL_FREQ (1843200UL)	  
//#define     SC16IS750_CRYSTCAL_FREQ (16000000UL)    
//#define     SC16IS750_DEBUG_PRINT   (0)

//Send command by i2c or spi
#define     SC16IS750_PROTOCOL_I2C  (0)
#define     SC16IS750_PROTOCOL_SPI  (1)

//Pin define 
#define	SC16IS75x_I2C_INTERFACE SS_IIC_0_ID
#define SC16IS75x_SPI_INTERFACE USE_DW_SPI_MST_1	
#define	SC16IS75x_BT_IRQ	IOMUX_PGPIO14
#define SC16IS75x_WIFI_IRQ	IOMUX_PGPIO13

//UART_A, UART_B
#define CH_A	1
#define CH_B	0

#define OUTPUT	1
#define INPUT	0
#define HIGH	1
#define LOW	0

#define GPIO0 0
#define GPIO1 1
#define GPIO2 2
#define GPIO3 3
#define GPIO4 4
#define GPIO5 5
#define GPIO6 6
#define GPIO7 7

#endif





    
    
