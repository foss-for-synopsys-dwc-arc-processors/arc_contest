#include "synopsys_wei_delay.h"

void hal_delay_ms(unsigned int input_ms)
{
    for(int delay_i = 0; delay_i < input_ms; delay_i ++)
      board_delay_cycle(CPU_CLK / 1000);
}
