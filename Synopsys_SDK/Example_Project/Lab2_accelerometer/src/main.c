#include "hx_drv_tflm.h"
#include "synopsys_wei_delay.h"

#include "stdio.h"

uint8_t string_buf[100] = "test\n";

#define accel_scale 10

typedef struct
{
	uint8_t symbol;
	uint32_t int_part;
	uint32_t frac_part;
} accel_type;


int main(int argc, char* argv[])
{
	int32_t int_buf;
	accel_type accel_x, accel_y, accel_z;
	uint32_t msec_cnt = 0;
	uint32_t sec_cnt = 0;

	hx_drv_uart_initial(UART_BR_115200);

	//It will initial accelerometer with sampling rate 119 Hz, bandwidth 50 Hz, scale selection 4g at continuous mode.
	//Accelerometer operates in FIFO mode. 
	//FIFO size is 32
	if (hx_drv_accelerometer_initial() != HX_DRV_LIB_PASS)
		hx_drv_uart_print("Accelerometer Initialize Fail\n");
	else
		hx_drv_uart_print("Accelerometer Initialize Success\n");

	while (1) 
	{
		hx_drv_uart_print("Testing: %d.%d sec\n", sec_cnt, msec_cnt / 100);

		uint32_t available_count = 0;
		float x, y, z;
		available_count = hx_drv_accelerometer_available_count();				
		hx_drv_uart_print("Accel get FIFO: %d\n", available_count);
		for (int i = 0; i < available_count; i++) 
		{
			hx_drv_accelerometer_receive(&x, &y, &z);
		}

		int_buf = x * accel_scale; //scale value
		if(int_buf < 0)
		{
			int_buf = int_buf * -1;
			accel_x.symbol = '-';
		}
		else 
		{
			accel_x.symbol = '+';
		}
		accel_x.int_part = int_buf / accel_scale;
		accel_x.frac_part = int_buf % accel_scale;


		int_buf = y * accel_scale; //scale value
		if(int_buf < 0)
		{
			int_buf = int_buf * -1;
			accel_y.symbol = '-';
		}
		else 
		{
			accel_y.symbol = '+';
		}
		accel_y.int_part = int_buf / accel_scale;
		accel_y.frac_part = int_buf % accel_scale;


		int_buf = z * accel_scale; //scale value
		if(int_buf < 0)
		{
			int_buf = int_buf * -1;
			accel_z.symbol = '-';
		}
		else 
		{
			accel_z.symbol = '+';
		}
		accel_z.int_part = int_buf / accel_scale;
		accel_z.frac_part = int_buf % accel_scale;



		sprintf(string_buf, "%c%1d.%1d | %c%1d.%1d | %c%1d.%1d G\n", 
				accel_x.symbol, accel_x.int_part, accel_x.frac_part, 
				accel_y.symbol, accel_y.int_part, accel_y.frac_part, 
				accel_z.symbol, accel_z.int_part, accel_z.frac_part);
		hx_drv_uart_print(string_buf);


		hx_drv_uart_print("\n\n");

		hal_delay_ms(100);
		msec_cnt = msec_cnt + 125;
		sec_cnt = sec_cnt + (msec_cnt / 1000);
		msec_cnt = msec_cnt % 1000;
	}
}
