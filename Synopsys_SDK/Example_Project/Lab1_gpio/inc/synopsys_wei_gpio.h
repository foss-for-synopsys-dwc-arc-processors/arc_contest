#ifndef ARC_GPIO_H
#define ARC_GPIO_H

#include "hx_drv_tflm.h"
typedef enum
{
    HAL_OK = 0,
    HAL_ERROR = 1
}
HAL_RETURN;

typedef enum
{
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET = 1
}
GPIO_PIN_DATA;

extern hx_drv_gpio_config_t hal_gpio_0;
extern hx_drv_gpio_config_t hal_gpio_1;
extern hx_drv_gpio_config_t hal_gpio_2;
extern hx_drv_gpio_config_t hal_led_r;
extern hx_drv_gpio_config_t hal_led_g;

HAL_RETURN hal_gpio_init(hx_drv_gpio_config_t * hal_gpio, HX_DRV_GPIO_E hal_gpio_pin, HX_DRV_GPIO_DIRCTION_E hal_gpio_dir, GPIO_PIN_DATA hal_gpio_data);
HAL_RETURN hal_gpio_set(hx_drv_gpio_config_t * hal_gpio, GPIO_PIN_DATA gpio_pin_data);
HAL_RETURN hal_gpio_get(hx_drv_gpio_config_t * hal_gpio, GPIO_PIN_DATA * gpio_pin_data);

#endif /* ARC_GPIO_H */

