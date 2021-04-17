#include "synopsys_wei_gpio.h"


HAL_RETURN hal_gpio_init(hx_drv_gpio_config_t * hal_gpio, HX_DRV_GPIO_E hal_gpio_pin, HX_DRV_GPIO_DIRCTION_E hal_gpio_dir, GPIO_PIN_DATA hal_gpio_data)
{
  hal_gpio->gpio_data = hal_gpio_data;
  hal_gpio->gpio_direction = hal_gpio_dir;
  hal_gpio->gpio_pin = hal_gpio_pin;
  if(hx_drv_gpio_initial(hal_gpio) != HX_DRV_LIB_PASS)
    return HAL_ERROR;
  else
    return HAL_OK;
}

HAL_RETURN hal_gpio_set(hx_drv_gpio_config_t * hal_gpio, GPIO_PIN_DATA gpio_pin_data)
{
  hal_gpio->gpio_data = gpio_pin_data;
  if(hx_drv_gpio_set(hal_gpio) != HX_DRV_LIB_PASS)
    return HAL_ERROR;
  else
    return HAL_OK;
}

HAL_RETURN hal_gpio_get(hx_drv_gpio_config_t * hal_gpio, GPIO_PIN_DATA * gpio_pin_data)
{
  if(hx_drv_gpio_get(hal_gpio) != HX_DRV_LIB_PASS)
      return HAL_ERROR;
  else
  {
    *gpio_pin_data = hal_gpio->gpio_data;
    return HAL_OK;
  }
}