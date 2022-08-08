#include "SC16IS750_Bluepacket.h"
#include "joystick.h"
#include <string.h>
#define ADC_3021_DEV_ADDR 0x4f
enum State { Up = 0, Idle = 7, Down = 15 }; // 3.3V

uint8_t rw_buf[4];
uint8_t joy_stick_VRY_addr = ADC_3021_DEV_ADDR;

int get_joystick_state() {
  memset(rw_buf, '\0', sizeof(rw_buf));
  hx_drv_i2cm_read_data(SS_IIC_0_ID, joy_stick_VRY_addr, rw_buf, 2);
  // printf("cur read: 0x%hx %hx\n", rw_buf[0], rw_buf[1]);
  switch ((int)rw_buf[0]) {
  case Down:
    return 1;
  case Up:
    return -1;
  case Idle:
  default:
    return 0;
  }
}

char *show_joystick_state() {
  int state = get_joystick_state();
  switch (state) {
  case 0:
    return "Idle";
  case -1:
    return "Up";
  case 1:
  default:
    return "Down";
  }
}

bool get_joystick_btn(uint8_t joy_stick_btn_pin) {
  return GPIOGetPinState(SC16IS750_PROTOCOL_SPI, CH_A, joy_stick_btn_pin) ^ 1;
  // btn's volt is high if btn is NOT pressed
  // By adding xor 1, the function will return 0 if btn is NOT pressed
}
