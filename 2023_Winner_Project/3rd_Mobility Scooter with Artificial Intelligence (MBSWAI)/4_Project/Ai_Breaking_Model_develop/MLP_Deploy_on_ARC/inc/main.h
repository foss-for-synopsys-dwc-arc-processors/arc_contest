#ifndef main_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "embARC.h"
#include "embARC_debug.h"
#include "board_config.h"
#include "arc_timer.h"
#include "hx_drv_spi_s.h"
#include "spi_slave_protocol.h"
#include "hardware_config.h"

#include "hx_drv_uart.h"
#define uart_buf_size 100

#define main_H_

#ifdef __cplusplus
extern "C"{
#endif

/*這裡是頭文件內容*/
/*頭文件內容結束*/


extern DEV_UART * uart0_ptr;
extern char uart_buf[uart_buf_size];


#ifdef __cplusplus
}
#endif

#endif

