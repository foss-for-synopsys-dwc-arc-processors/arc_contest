#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "embARC.h"
#include "embARC_debug.h"
#include "board_config.h"
#include "arc_timer.h"
#include "hardware_config.h"

#include "hx_drv_iic_m.h"
#include "hx_drv_iomux.h"
#include "SC16IS750_Bluepacket.h"

#include "hx_drv_uart.h"

#include "tflitemicro_algo.h"
#include "model_settings.h"
#include "test_samples.h"
#include "test_samples_val.h"
#include "test_samples_val_raw.h"

#include "self_define_type.h"
#include <ctype.h>

float input_buf[kInstanceSize] = {0};

void getMinMax(Acc_Proc *ptr, unsigned int buff_size, Acc_Proc *min, Acc_Proc *max)
{
	min->x = ptr[0].x;
	min->y = ptr[0].y;
	min->z = ptr[0].z;
	max->x = ptr[0].x;
	max->y = ptr[0].y;
	max->z = ptr[0].z;

	for (int i = 1; i < buff_size; i++)
	{
		min->x = (ptr[i].x < min->x) ? ptr[i].x : min->x;
		min->y = (ptr[i].y < min->y) ? ptr[i].y : min->y;
		min->z = (ptr[i].z < min->z) ? ptr[i].z : min->z;
		max->x = (ptr[i].x > max->x) ? ptr[i].x : max->x;
		max->y = (ptr[i].y > max->y) ? ptr[i].y : max->y;
		max->z = (ptr[i].z > max->z) ? ptr[i].z : max->z;
	}
}

void minMaxScaler(Acc_Proc *ptr, unsigned int buff_size)
{
	Acc_Proc min, max;
	getMinMax(ptr, buff_size, &min, &max);

	printf("min %.2f %.2f %.2f\r\n", min.x, min.y, min.z);
	printf("max %.2f %.2f %.2f\r\n", max.x, max.y, max.z);

	for (int i = 0; i < buff_size; i++)
	{
		ptr[i].x = (ptr[i].x - min.x) / (max.x - min.x);
		ptr[i].y = (ptr[i].y - min.y) / (max.y - min.y);
		ptr[i].z = (ptr[i].z - min.z) / (max.z - min.z);
	}
}

void makeTimeSteps(Acc_Proc *start_ptr)
{
	for (int i = 0; i < kTimeSteps; i++)
	{
		input_buf[kNumOfDim * i + 0] = start_ptr[i].x;
		input_buf[kNumOfDim * i + 1] = start_ptr[i].y;
		input_buf[kNumOfDim * i + 2] = start_ptr[i].z;
	}
}

void testTFLite(void)
{
	int correct = 0;
	for (int i = 0; i < kNumSamples; i++)
	{
		int ret = tflitemicro_algo_run((float *)test_samples[i].data);
		printf("Round %2d: %d\r\n", i, ret);
		if (test_samples[i].label == ret)
		{
			correct += 1;
		}
	}
	printf("%f %%  (%d/%d)\r\n", (float)correct / (float)kNumSamples, correct, kNumSamples);
}

void valTFLite(void)
{
	for (int i = 0; i < 512; i++)
	{
		int ret = tflitemicro_algo_run((float *)val_samples[i]);
		printf("Round %2d: %d\r\n", i, ret);
	}
}

int main(void)
{
	printf("Gait Pod Project! @by Young Boy!\r\n");

	HX_GPIOSetup();
	IRQSetup();
	UartInit(SC16IS750_PROTOCOL_SPI);
	GPIOSetPinMode(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, INPUT);
	tflitemicro_algo_init();
	printf("Board init finish...\r\n");
	// testTFLite();
	// valTFLite();


	// +回來!
	Acc_Proc *gait_proc = ReadFromUart_ParseToFloat(SC16IS750_PROTOCOL_SPI);
	// Acc_Proc gait_proc[532];
	// for (int i = 0; i < 532; i++)
	// {
	// 	gait_proc[i].x = xx[i];
	// 	gait_proc[i].y = yy[i];
	// 	gait_proc[i].z = zz[i];
	// }

	printf("scale to (0, 1)\r\n");
	minMaxScaler(gait_proc, fBUFF_SIZE);
	for (int i = 0; i < fBUFF_SIZE; i++)
	{
		printf("idx%d " NRF_LOG_FLOAT_MARKER " " NRF_LOG_FLOAT_MARKER " " NRF_LOG_FLOAT_MARKER "\r\n", i, NRF_LOG_FLOAT(gait_proc[i].x), NRF_LOG_FLOAT(gait_proc[i].y), NRF_LOG_FLOAT(gait_proc[i].z));
	}

	// *** slice data into time slice and run tflite inference ***
	for (int i = 0; i < fBUFF_SIZE - kTimeSteps; i++)
	{
		makeTimeSteps(gait_proc + i);
		int ret = tflitemicro_algo_run(input_buf);
		printf("Round %2d: %d\r\n", i, ret);
	}
	printf("THE END\r\n");

	// InitGPIOSetup(SC16IS750_PROTOCOL_SPI);
	// StartTestCMD(SC16IS750_PROTOCOL_SPI);
	// TestGPIO();
	return 0;
}
