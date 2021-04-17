/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "main_functions.h"

#include "hx_drv_tflm.h"
#include "stdio.h"

volatile void delay_ms(uint32_t ms_input);

char string_buf[100] = "test\n";


// This is the default main used on systems that have the standard C entry
// point. Other devices (for example FreeRTOS or ESP32) that have different
// requirements for entry code (like an app_main function) should specialize
// this main.cc file in a target-specific subfolder.
int main(int argc, char* argv[]) 
{
    uint32_t msec_x100 = 0;

    hx_drv_uart_initial(UART_BR_115200);
    hx_drv_uart_print("Testing\n");

    setup();

    while (true) 
    {
        hx_drv_uart_print("Start time count: %5d.%1d\n", msec_x100 / 10, msec_x100 % 10);
        hx_drv_uart_print("Detecting\n");
        loop();

        msec_x100 = msec_x100 + 5;
        delay_ms(500);
    }
}


volatile void delay_ms(uint32_t ms_input)
{
    volatile uint32_t i = 0;
    volatile uint32_t j = 0;

    for(i = 0; i < ms_input; i++)
        for(j = 0; j < 40000; j++);
}
