#include "hx_drv_tflm.h"
#include "synopsys_wei_delay.h"

#include "stdio.h"
#include "string.h"

uint8_t string_buf[100] = "test\n";

hx_drv_sensor_image_config_t pimg_config;

int main(int argc, char* argv[])
{
	uint8_t key_data;

	hx_drv_uart_initial(UART_BR_115200);


 	//sensor start capture and start streaming
 	if(hx_drv_sensor_initial(&pimg_config) == HX_DRV_LIB_PASS)
	 	hx_drv_uart_print("Camera Initialize Successful\n");
/*
	//JPEG image data is ready at memory address "pimg_config.jpeg_address"
	//send JPEG image out via SPI master
	if(hx_drv_spim_send(pimg_config.jpeg_address, pimg_config.jpeg_size, SPI_TYPE_JPG) != HX_DRV_LIB_PASS)

	//RAW image data is ready at memory address "raw_address"
	//send RAW image out via SPI master
	if(hx_drv_spim_send(pimg_config.raw_address, pimg_config.raw_size, SPI_TYPE_RAW) != HX_DRV_LIB_PASS)
*/

	hx_drv_uart_print("Wait for user press key: [A] \n");
	while (1) 
	{
		hx_drv_uart_getchar(&key_data);
		if(key_data == 'A')
		{		
			if(hx_drv_sensor_capture(&pimg_config) == HX_DRV_LIB_PASS)
			{
				hx_drv_uart_print("Camera Get Data Success\n");

				uint8_t * img_ptr;
				img_ptr = (uint8_t *) pimg_config.raw_address;
				hx_drv_uart_print("Start to send \n");
				hx_drv_uart_print("Image width: %d\n", pimg_config.img_width);
				hx_drv_uart_print("Image height: %d\n", pimg_config.img_height);
				hx_drv_uart_print("Image size: %d Bytes\n", pimg_config.raw_size);
				hx_drv_uart_print("Image address: 0x%08x\n", img_ptr);

				for(uint32_t heigth_cnt = 0; heigth_cnt < pimg_config.img_height; heigth_cnt ++)
				{
					for(uint32_t width_cnt = 0; width_cnt < pimg_config.img_width; width_cnt ++)
					{
						hx_drv_uart_print("%3d", *img_ptr);
						if(width_cnt != (pimg_config.img_width - 1))
							hx_drv_uart_print(", ");
						else
							hx_drv_uart_print("\n");

						img_ptr = img_ptr + 1;
					}
				}
				hx_drv_uart_print("End of send\n");
			}
		}
		key_data = '\0';
	}
}
