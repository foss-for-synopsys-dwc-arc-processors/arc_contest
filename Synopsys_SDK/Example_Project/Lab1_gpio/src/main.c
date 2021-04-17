#include "hx_drv_tflm.h"
#include "synopsys_wei_delay.h"
#include "synopsys_wei_gpio.h"

hx_drv_gpio_config_t hal_gpio_0;
hx_drv_gpio_config_t hal_gpio_1;
hx_drv_gpio_config_t hal_gpio_2;
hx_drv_gpio_config_t hal_led_r;
hx_drv_gpio_config_t hal_led_g;

void GPIO_INIT(void);

int main(int argc, char* argv[])
{
  
  uint32_t sec_cnt = 0;

  hx_drv_uart_initial(UART_BR_115200);

  GPIO_INIT();

  while (1) 
  {
    hx_drv_uart_print("Testing: %d sec\n", sec_cnt);

    uint8_t gpio_level;
    hal_gpio_get(&hal_gpio_0, &gpio_level);
    if(gpio_level == 0)
      hx_drv_uart_print("GPIO_0 Logic: Low\n\n");
    else
      hx_drv_uart_print("GPIO_0 Logic: High\n\n");

    switch(sec_cnt % 4)
    {
      case 0:
        hal_gpio_set(&hal_led_g, GPIO_PIN_RESET);
        hal_gpio_set(&hal_led_r, GPIO_PIN_RESET);
        break;    
      case 1:
        hal_gpio_set(&hal_led_g, GPIO_PIN_RESET);
        hal_gpio_set(&hal_led_r, GPIO_PIN_SET);
        break;      
      case 2:
        hal_gpio_set(&hal_led_g, GPIO_PIN_SET);
        hal_gpio_set(&hal_led_r, GPIO_PIN_RESET);
        break;      
      case 3:
        hal_gpio_set(&hal_led_g, GPIO_PIN_SET);
        hal_gpio_set(&hal_led_r, GPIO_PIN_SET);
        break;      
      default:
        break;  
    }
    hal_delay_ms(1000);
    sec_cnt ++;
  }
}

void GPIO_INIT(void)
{
  if(hal_gpio_init(&hal_gpio_0, HX_DRV_PGPIO_0, HX_DRV_GPIO_INPUT, GPIO_PIN_RESET) == HAL_OK)
    hx_drv_uart_print("GPIO0 Initialized: OK\n");
  else
    hx_drv_uart_print("GPIO0 Initialized: Error\n");

  if(hal_gpio_init(&hal_gpio_1, HX_DRV_PGPIO_1, HX_DRV_GPIO_INPUT, GPIO_PIN_RESET) == HAL_OK)
    hx_drv_uart_print("GPIO1 Initialized: OK\n");
  else
    hx_drv_uart_print("GPIO1 Initialized: Error\n");

  if(hal_gpio_init(&hal_gpio_2, HX_DRV_PGPIO_2, HX_DRV_GPIO_INPUT, GPIO_PIN_RESET) == HAL_OK)
    hx_drv_uart_print("GPIO2 Initialized: OK\n");
  else
    hx_drv_uart_print("GPIO2 Initialized: Error\n");

  if(hal_gpio_init(&hal_led_r, HX_DRV_LED_RED, HX_DRV_GPIO_OUTPUT, GPIO_PIN_RESET) == HAL_OK)
    hx_drv_uart_print("GPIO_LED_RED Initialized: OK\n");
  else
    hx_drv_uart_print("GPIO_LED_RED Initialized: Error\n");

  if(hal_gpio_init(&hal_led_g, HX_DRV_LED_GREEN, HX_DRV_GPIO_OUTPUT, GPIO_PIN_RESET) == HAL_OK)
    hx_drv_uart_print("GPIO_LED_GREEN Initialized: OK\n");
  else
    hx_drv_uart_print("GPIO_LED_GREEN Initialized: Error\n");
}
