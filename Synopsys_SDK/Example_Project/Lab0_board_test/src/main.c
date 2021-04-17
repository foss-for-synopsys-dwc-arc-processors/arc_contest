#include "hx_drv_tflm.h"
#include "synopsys_wei_delay.h"

int main(int argc, char* argv[])
{
  uint32_t sec_cnt = 0;

  hx_drv_uart_initial(UART_BR_115200);
  
  while (1) 
  {
    hx_drv_uart_print("Testing: %d sec\n", sec_cnt);

    hal_delay_ms(1000);
    sec_cnt ++;
  }
}
