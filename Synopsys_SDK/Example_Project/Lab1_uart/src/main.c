#include "hx_drv_tflm.h"
#include "synopsys_wei_delay.h"
#include "synopsys_wei_uart.h"

#define uart_buf_size 100

uint8_t uart_rx_flag = 0;
uint8_t uart_rx_cnt = 0;
uint8_t uart_rx_str[uart_buf_size] = {0};

int main(int argc, char* argv[])
{

  hx_drv_uart_initial(UART_BR_115200);
  hx_drv_uart_print("URAT_GET_STRING_START\n");
  
  while (1) 
  {
    /****************2021_0317********************
      When ARC_TOOLCHAIN=gnu

      hx_drv_uart_getchar(uint8_t var*)
      This var can't use global var.
      It got sometime wrong.
    ***************2021_0317********************/
    uint8_t uart_rx_char = 0;
    if(hal_uart_get_char(&uart_rx_char) == HAL_OK)
    {
      hx_drv_uart_print("Echo char: 0x%02x\n", uart_rx_char);
      hx_drv_uart_print("String cnt: %d\n\n", uart_rx_cnt);

      uart_rx_str[uart_rx_cnt] = uart_rx_char;

      uart_rx_cnt ++;
      if(uart_rx_cnt >= uart_buf_size)
        uart_rx_cnt = uart_buf_size - 1;

      if(uart_rx_char == 0x0d)
      {
        uart_rx_flag = 1;
        uart_rx_str[uart_rx_cnt] = '\0';
        uart_rx_cnt = 0;
      }   
    }

    if(uart_rx_flag == 1)
    {
      uart_rx_flag = 0;
      hx_drv_uart_print("\n\n\n");
      hx_drv_uart_print("Echo string: %s\n", uart_rx_str);
    }
  }
}