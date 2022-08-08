#ifndef _ADC_JOYSTICK_H_
#define _ADC_JOYSTICK_H_
#include "hx_drv_iic_m.h"

#define JoyBtn 5 // Set gpio5 as JoyBtn pin
#define JoyVRx 6 // Set gpio6 as JoyVRx pin,
                 // mind we connected it to a
                 // digital pin, thus it would
                 // be treated as another btn

/*
 *  1 - Up
 *  0 - Idle
 * -1 - Down
 */
int get_joystick_state();

// Print joystick state in String
char *show_joystick_state();
bool get_joystick_btn(uint8_t);
#endif