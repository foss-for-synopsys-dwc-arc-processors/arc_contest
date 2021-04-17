#include "hx_drv_tflm.h"
#include "synopsys_wei_delay.h"
#include "synopsys_wei_i2c_oled1306.h"

#include "stdio.h"

uint8_t string_buff[900*1024] = {0};

int main(int argc, char* argv[])
{
	uint8_t oled_i = 0;
	uint32_t msec = 0;
	uint32_t sec = 0;

	hx_drv_share_switch(SHARE_MODE_I2CM);

	OLED_Init();
	OLED_Clear();

	OLED_SetCursor(0, 0);
	for(oled_i = 0; oled_i < 128; oled_i ++)
		oledSendData(oled_i);

	OLED_SetCursor(1, 5);
	for(oled_i = 0; oled_i < 20; oled_i ++)
		OLED_DisplayChar(0x20 + oled_i);

	OLED_SetCursor(2, 10);
	for(oled_i = 0; oled_i < 13; oled_i ++)
		OLED_DisplayChar('A' + oled_i);

	OLED_SetCursor(3, 15);
	for(oled_i = 0; oled_i < 13; oled_i ++)
		OLED_DisplayChar('A' + oled_i + 13);

	OLED_SetCursor(4, 20);
	for(oled_i = 0; oled_i < 13; oled_i ++)
		OLED_DisplayChar('a' + oled_i);

	OLED_SetCursor(5, 25);
	for(oled_i = 0; oled_i < 13; oled_i ++)
		OLED_DisplayChar('a' + oled_i + 13);

	while (1) 
	{
		sprintf(string_buff, "Time Cnt: %4d.%03d", sec, msec);

		OLED_SetCursor(7, 0);
		OLED_DisplayString(string_buff);

		msec = msec + 17;
		sec = sec + (msec / 1000);
		msec = msec % 1000;
		hal_delay_ms(10);
	}	
}
