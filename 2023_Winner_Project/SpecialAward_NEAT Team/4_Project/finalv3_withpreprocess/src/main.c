#include "main.h"

// #include "hx_drv_iic_m.h"
// #include "hx_drv_iomux.h"
#include "SC16IS750_Bluepacket.h"
#include "tflitemicro_algo.h"
#include "model_settings.h"
#include "hx_drv_iomux.h"
DEV_UART * uart0_ptr;
char uart_buf[uart_buf_size] = {0};
float rssi_buf[69] = {0};
float acc_buf[69] = {0};
char gpio_value;
volatile uint8_t gpio_out_state = 0;
volatile uint8_t gpio_in_value;
int c=0;
int test_int=0;
int test_int_acc=0;
float rssi_buf_convert[60]={0};
float acc_buf_convert[60]={0};
float Rssi_tmp=0;
int main(void)
{    
	//UART 0 is already initialized with 115200bps
	int i,j;
	int test_result = 0;
    uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);

    //tflitemicro_algo_init();
	HX_GPIOSetup();
	IRQSetup();
	UartInit(SC16IS750_PROTOCOL_SPI);	

    printf("Initial\r\n");
	test_UART_Read(rssi_buf,acc_buf);
	for(i=0;i<69;i++)
	{
		test_int=rssi_buf[i]*100;
		test_int_acc=acc_buf[i]*100;
		// printf("%d\r\n",i);
		printf("rssi : %d , ",test_int);
		printf("acc : %d \r\n",test_int_acc);
	}
	//For Rssi preprocess
	//j表示convert buf要幾筆
    for(j=0;j<60;j++)
    {   Rssi_tmp=0;
    //i表示要幾筆做平均
        for(i=j;i<10+j;i++)
        {
            
            Rssi_tmp+=rssi_buf[i];
            
        }
    
        //printf("%.2f\n",tmp);
        rssi_buf_convert[j]=Rssi_tmp/10.0;
		test_int=rssi_buf_convert[j]*100;
		printf("rssi_buf_convert:%d\r\n",test_int);
        //printf("%.2f\n",rssi_buf_convert[j]);
        
    }
	printf("Rssi_buf_convert len:%d\r\n",strlen(rssi_buf_convert));	
	//For Acc preprocess	
	for(i=0;i<60;i++)
	{
		acc_buf_convert[i]=acc_buf[i];
	}
	//
	printf("acc_buf_convert len:%d\r\n",strlen(acc_buf_convert));
	tflitemicro_algo_init();
	test_result = tflitemicro_algo_run(rssi_buf_convert);
	printf("Rssi Pred: %2d\r\n",test_result);
	hx_drv_iomux_set_pmux(IOMUX_PGPIO8, 3); //Extension Board Red LED Output
    hx_drv_iomux_set_pmux(IOMUX_PGPIO9, 3); //Extension Board Blue LED Output
    hx_drv_iomux_set_pmux(IOMUX_PGPIO12, 3); //Extension Board Green LED Output

	if(test_result == 1)
	{
		hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 1);
	}
	else
	{
		hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 0);
	}

        hx_drv_iomux_get_invalue(IOMUX_PGPIO9, &gpio_value);
        if(gpio_value == 1)
        {
            sprintf(uart_buf, "Get GPIO9 Logic: High\r\n\n");
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);
        }   
        else 
        {
            sprintf(uart_buf, "Get GPIO0 Logic: Low\r\n\n");
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);
        }

	tflitemicro_algo_init_acc();
	test_result = tflitemicro_algo_run_acc(acc_buf_convert);
	printf("Acc Pred: %2d\r\n",test_result);
	if(test_result == 1)
	{
		hx_drv_iomux_set_outvalue(IOMUX_PGPIO12, 1);
	}
	else
	{
		hx_drv_iomux_set_outvalue(IOMUX_PGPIO12, 0);
	}               
        hx_drv_iomux_get_invalue(IOMUX_PGPIO12, &gpio_value);
        if(gpio_value == 1)
        {
            sprintf(uart_buf, "Get GPIO12 Logic: High\r\n\n");
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);
        }
        else 
        {
            sprintf(uart_buf, "Get GPIO12 Logic: Low\r\n\n");
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);
        }	
	return 0;
}
