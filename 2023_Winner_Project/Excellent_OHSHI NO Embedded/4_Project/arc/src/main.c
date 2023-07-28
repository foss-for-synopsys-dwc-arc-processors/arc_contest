#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "embARC.h"
#include "embARC_debug.h"
#include "board_config.h"
#include "arc_timer.h"
#include "hx_drv_spi_s.h"
#include "spi_slave_protocol.h"
#include "hardware_config.h"

#include "hx_drv_uart.h"


#include "max86150.h"
#include "tmp117.h"
#include "pca9672.h"
#include "SC16IS750_Bluepacket.h"
#include "st7735.h"
#include "sd178b.h"
#include "tflitemicro_algo.h"
#include "model_settings.h"
DEV_UART * uart0_ptr;
//char uart0_buf[101] = "BLEXOKPXXXLXXAXXXBXXXCXXXDXXXEXXXFXXXPXXXLXXAXXXBXXXCXXXDXXXEXXXFXXXPXXXLXXAXXXBXXXCXXXDXXXEXXXFXXXED";
//                           PXXXLXXAXXXBXXXCXXXDXXXEXXXFXXX
//                           PXXXLXXAXXXBXXXCXXXDXXXEXXXFXXX
//char uart0_buf[] = "BLE3011aaabbbcccdddeeefff022aaabbbcccdddeeefff033aaabbbcccdddeeefff";
/*DEV_UART * uart1_ptr;
char uart1_buf[100] = {0};*/
DEV_IIC * iic0_ptr;
DEV_IIC * iic1_ptr;
DEV_SPI * spi_ptr;
//float hr_b[9] = {0.1,  0.1, -0.1,   0.1,  0.1,   0.1, -0.1,  0.1, 0.1};
//float hr_a[9] = {1.0000, -7.8176, 26.7439, -52.2931, 63.9219, -50.0198, 24.4694, -6.8419, 0.8372};
int32_t hr_a[9]={1 ,0 ,-4 ,0 ,5 ,0 ,-4 ,0 ,1};
int32_t hr_b[9]={100000 ,-7817568 ,26743892 ,-52293061 ,63921867 ,-50019813 ,24469401 ,-6841897 ,837181};
int32_t sp2O_b[6] = {0.9970, -4.9848, 9.9696, -9.9696, 4.9848, -0.9970};
int32_t sp2O_a[6] = {1.0000, -4.9939, 9.9756, -9.9635, 4.9757, -0.9939};
	uint32_t 				  PPGIR [1000] = {0};//880nm  FIFO DATA[23:0]
	uint32_t 				  PPGRED[1000] = {0};//660nm
    uint32_t                baseline[1000] = {0};//基線值
	 int16_t                ppgir_ac[1000] = {0};//IR去基線
	 int16_t               ppgred_ac[1000] = {0};//RED去基線
	 int16_t           smootg_signal[1000] = {0};//濾波後

	 uint8_t   Lock         =  0 ; //28 or 12
	 uint8_t   State        =  0 ;
	 uint8_t 	Lv			=  0 ;
     uint8_t    SOC         =  0 ;
     uint8_t    HR          =  1 ;
     uint8_t    HR_T        =  0 ;
     int      SP2O          =  0 ;
	uint32_t   BodTemp      =  0 ;

#define KERNEL_SIZE 200
int half_kernel_size = KERNEL_SIZE / 2;
uint32_t kernel[KERNEL_SIZE];
void medfilt(uint32_t *input, int n, uint32_t *output) {
    int i, j,k;
    for (i = 0; i < n; i++) {
        for (j = 0; j < KERNEL_SIZE; j++) {
            kernel[j] = input[i + j - half_kernel_size];
        }
        for (j = 0; j < KERNEL_SIZE - 1; j++) {
            for (k = j + 1; k < KERNEL_SIZE; k++) {
                if (kernel[j] > kernel[k]) {
                    uint32_t temp = kernel[j];
                    kernel[j] = kernel[k];
                    kernel[k] = temp;
                }
            }
        }
        output[i] = kernel[half_kernel_size];
    }
}
int16_t minNegative =0; 
//int16_t absMinNegative =0;
void adjustArray(int16_t arr[], int size) {
    // 找到最小负值// 初始化为一个较大的正数
    for (int i = 0; i < size; ++i) {
        if (arr[i] < 0 && arr[i] < minNegative) {
            minNegative = arr[i];
        }
    }
    // 将数组中的所有值增加最小负值的绝对值
    minNegative = abs(minNegative);
    for (int i = 0; i < size; ++i) {
        arr[i] += minNegative;
    }
}
int32_t mva_sum;
void moving_average(const int16_t* arr, int size, int window_size, int16_t* output) {
    int i, j;

    for (i = 0; i < size - window_size + 1; ++i) {
        mva_sum = 0;
        for (j = 0; j < window_size; ++j) {
            mva_sum += arr[i + j];
        }
        output[i] = (int16_t)(mva_sum / window_size);
    }
}
int32_t mva_sum_ml;
void moving_average_ml(const int32_t* arr, int size, int window_size, int32_t* output) {
    int i, j;
    for (i = 0; i < size - window_size + 1; ++i) {
        mva_sum = 0.0;
        for (j = 0; j < window_size; ++j) {
            mva_sum += arr[i + j];
        }
        output[i] = (int32_t)(mva_sum / window_size);
    }
}
int   num_peaks    =  0 ;  
int16_t   peaks[100]   = {0};
void find_peaks(const int16_t y[], int n, int height, int16_t peaks[], int *num_peaks) {
    *num_peaks = 0;

    // 处理第一个元素
    if (n > 0 && y[0] > height && y[0] > y[1]) {
        peaks[(*num_peaks)++] = 0;
    }

    // 处理中间元素
    for (int i = 1; i < n - 1; i++) {
        if (y[i] >= height && y[i] > y[i - 1] && y[i] > y[i + 1]) {
            peaks[(*num_peaks)++] = i;
        }
    }

    // 处理最后一个元素
    if (n > 1 && y[n - 1] > height && y[n - 1] > y[n - 2]) {
        peaks[(*num_peaks)++] = n - 1;
    }
}
/************/
float heart_rate = 0; 
float sum_peak_time = 0;
float error_find[100]= {0};
float peak_time[100]={0};
float sum_error_time = 0;
int error_find_len = 0;
/************/
int64_t R[1000] ={0};
float R_Temp =0;
int64_t sp2o_errorpick[1000] = {0};
int errorpick_index = 0;
int64_t sp2o_errorpick_sum = 0;
int    SP2=  0 ;
int64_t r1,r2,ss,X1,X2;
int64_t MA,MB,MC,MD;
/************/
int32_t ppgir_step1[1000] = {0};
int32_t ppgir_step2[1000] = {0};
double ppgir_step3[1000] = {0};
double ppgir_ml[1000] = {0};
int tlmin[300]={0};
int tlmax[300]={0};
int32_t ppgmax=0;
int tlmin_count=0;
int tlmax_count=0;
int bp5=0;
int sut[150]={0};
int cp[150]={0};
int dt[150]={0};
int msut=0;
int mcp=0;
int mdt=0;
int sc,dc,cc=0;
double mdt1,mdt2,mcp1=0;
/************/
uint8_t machine_learning[6]={0};
int BPU,BPD=0;
/************/
double temp_ff[1000];
void filtfilt(double *input, double *output) {
    // 固定滤波器系数
    int m_b = 6;
    double b[6] ; 
    int m_a = 6;
    double a[6] ; 
    int n = 1000;
    a[0] = 1.0;
    a[1] = -3.98454312 ;
    a[2] = 6.43486709;
    a[3] = -5.25361517;
    a[4] = 2.16513291;
    a[5] = -0.35992825;
// 1.         -3.98454312  6.43486709 -5.25361517  2.16513291 -0.35992825]
//5.97957804e-05 2.98978902e-04 5.97957804e-04 5.97957804e-04
// 2.98978902e-04 5.97957804e-05]

    b[0] = 5.97957804e-05;
    b[1] = 2.98978902e-04;
    b[2] = 5.97957804e-04;
    b[3] = 5.97957804e-04;
    b[4] = 2.98978902e-04;
    b[5] = 5.97957804e-05;
   

    for (int i = 0; i < n; i++) {
        temp_ff[i] = b[0] * input[i];
        for (int j = 1; j < m_b; j++) {
            if (i - j >= 0) {
                temp_ff[i] += b[j] * input[i - j];
            }
        }
        for (int j = 1; j < m_a; j++) {
            if (i - j >= 0) {
                temp_ff[i] -= a[j] * temp_ff[i - j];
            }
        }
    }
    for (int i = n - 1; i >= 0; i--) {
        output[i] = b[0] * temp_ff[i];
        for (int j = 1; j < m_b; j++) {
            if (i + j < n) {
                output[i] += b[j] * temp_ff[i + j];
            }
        }
        for (int j = 1; j < m_a; j++) {
            if (i + j < n) {
                output[i] -= a[j] * output[i + j];
            }
        }
    }
}
/****************************/
double soc_temp_ff[1000];
void soc_filtfilt(double *input, double *output) {
    
    int m_b = 6;
    double b[6] ; 
    int m_a = 6;
    double a[6] ; 
    int n = 1000;
    b[0] = 1.15831407e-10;
    b[1] = 5.79157033e-10;
    b[2] = 1.15831407e-09;
    b[3] = 1.15831407e-09;
    b[4] = 5.79157033e-10;
    b[5] = 1.15831407e-10;
       
    a[0] = 1.0;
    a[1] = -4.93290206;
    a[2] = 9.73385381;
    a[3] = -9.60410274;
    a[4] = 4.7382529;
    a[5] = -0.9351019;
    for (int i = 0; i < n; i++) {
        soc_temp_ff[i] = b[0] * input[i];
        for (int j = 1; j < m_b; j++) {
            if (i - j >= 0) {
                soc_temp_ff[i] += b[j] * input[i - j];
            }
        }
        for (int j = 1; j < m_a; j++) {
            if (i - j >= 0) {
                soc_temp_ff[i] -= a[j] * soc_temp_ff[i - j];
            }
        }
    }
    for (int i = n - 1; i >= 0; i--) {
        output[i] = b[0] * soc_temp_ff[i];
        for (int j = 1; j < m_b; j++) {
            if (i + j < n) {
                output[i] += b[j] * soc_temp_ff[i + j];
            }
        }
        for (int j = 1; j < m_a; j++) {
            if (i + j < n) {
                output[i] -= a[j] * output[i + j];
            }
        }
    }
}
int rmax=0;
int lmax=0;
int rmin=0;
int lmin=0;
int rr_final=0;
double soc_d_in[1000]={0};
double soc_d_out[1000]={0};
int rrppg[1000]={0};
/********************************/
int sp2o_kk[6]={0};
int sp2o_flag=0;
int sp2o_n=0;
int hr_kk[6]={0};
int hr_flag=0;
int hr_n=0;
int main(void){
	uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);
    uart0_ptr->uart_open(UART_BAUDRATE_115200); //UART0 can't change baud
    /*
    uart1_ptr = hx_drv_uart_get_dev(USE_SS_UART_1);
    uart1_ptr->uart_open(UART_BAUDRATE_115200);//9600 
    */
    iic0_ptr  = hx_drv_i2cm_get_dev(USE_SS_IIC_0);
    iic0_ptr->iic_open(DEV_MASTER_MODE, IIC_SPEED_STANDARD);//100kbps PCA9672 MAX86150 
    iic1_ptr  = hx_drv_i2cm_get_dev(USE_SS_IIC_1);
    iic1_ptr->iic_open(DEV_MASTER_MODE, IIC_SPEED_STANDARD);//100kbps TMP117 Sd178b
	spi_ptr   = hx_drv_spi_mst_get_dev(USE_DW_SPI_MST_1);
	spi_ptr->spi_open(DEV_MASTER_MODE, 10000000);//10Mbps ST7735
	spi_ptr->spi_control(SPI_CMD_SET_CLK_MODE, SPI_CLK_MODE_0);  
	HX_GPIOSetup();
	IRQSetup();
	UartInit(SC16IS750_PROTOCOL_SPI);
	//InitGPIOSetup(SC16IS750_PROTOCOL_SPI);
	GPIOSetPinMode(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, OUTPUT);//PCA9672 RESET
	GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO2, HIGH);//PCA9672 RES=1
    board_delay_ms(1); //Delay 120ms
	hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 0);
	hx_drv_iomux_set_outvalue(IOMUX_PGPIO12, 1); //cs2
	Pca9672_Init();
	Max86150_Init();
    Max86150_GetPPG(50, PPGIR, PPGRED);
    Sd178b_Init();
	Tmp117_Init();
	St7735_Init();
    Sd178b_Call(0);
    printf("sd178b call end ");

	//sprintf(uart0_buf, "Start While Loop uart0\r\n");


//    uart0_ptr->uart_write(uart0_buf, strlen(uart0_buf));
//    printf("uart0 ..ok\n");



	//printf("------------------------- Main -------------------------\n");
	Pca9672_GetState(&State);
	Lock=State;
	//printf("Lock  =  %d\n",Lock);
	while(1){
		Pca9672_GetState(&State);
		if(Lock==State){
			while(1){
				Pca9672_GetState(&State);
				if(State == Lock - 8){
					if(Lv != 4)Lv = Lv + 1;	
					else Lv = 1;
					St7735_Display(Lv, 0, 0, 0, 0, 0, 0);
					break;
				}
				if(Lock == State - 16 || Lock == State + 16)
					break;
			}
		}
		else if(Lock == State - 16 || Lock == State + 16){
            memset(PPGIR, '\0', sizeof(PPGIR));
            memset(PPGRED, '\0', sizeof(PPGRED));
			Max86150_GetPPG(1000, PPGIR, PPGRED);//取樣RED、IR
            memset(soc_d_in, '\0', sizeof(soc_d_in));
            for(int i=0;i<1000;i++)
                soc_d_in[i]=(double)PPGIR[i];
            printf("rrppg\n\n");
            soc_filtfilt(soc_d_in,soc_d_out);
            memset(rrppg, '\0', sizeof(rrppg));
            for(int i=0;i<1000;i++){
                rrppg[i]=(int)soc_d_out[i];
               // printf("%d ,",rrppg[i]);
            }
            lmax=0;
            for(int i=50;i<950;i++){
                if (rrppg[i]>rmax){
                    rmax=rrppg[i];
                    lmax=i;
                }
            }
            rmin=rmax;
            lmin=lmax;
            for(int i=50;i<950;i++){
                if (rrppg[i]<rmin){
                    rmin=rrppg[i];
                    lmin=i;
                }
            }
            rr_final=0;
            rr_final=4000/abs(lmax-lmin);
            if (rr_final<10)
                rr_final=10;
            if (rr_final>40)
                rr_final=40;
            SOC=(uint8_t)(rr_final);
        printf("呼吸 = %d\n",SOC);
        /************************************/
            memset(baseline, '\0', sizeof(baseline));
            medfilt(PPGIR ,1000 ,baseline);//提取IR基線
            memset(ppgir_ac, '\0', sizeof(ppgir_ac));
            for (int i = 0; i < 1000; i++)
                ppgir_ac[i] = (int16_t)(PPGIR[i] - baseline[i]);//去除IR基線    
            //filtfilt(ppgir_ac, smootg_signal, 1000, hr_b, 9, hr_a, 9);
            //for (int i = 0; i < 1000; i++)
            //    printf("%d %d to %d to %d\n",i,PPGIR[i],ppgir_ac[i],smootg_signal[i]);
            adjustArray(ppgir_ac, 1000);//平移至0以上 
        //    printf("minNegative=%d\n",minNegative);
            memset(smootg_signal, '\0', sizeof(smootg_signal));
            moving_average(ppgir_ac, 1000 ,55 ,smootg_signal);//均值濾波
            memset(peaks, '\0', sizeof(peaks));
            find_peaks(smootg_signal, 1000 ,minNegative ,peaks ,&num_peaks);//計算鋒值點與量
        //printf("num_peaks=%d\n",num_peaks);
            heart_rate = (float)(peaks[num_peaks-1] - peaks[0]);
            heart_rate = 60.0 / ((heart_rate / (float)(num_peaks - 1)) / 200);//計算心率
        //    HR = (roundf(heart_rate * 10) / 10);
        //    printf("心率1 = %d\n",HR);
            memset(peak_time, '\0', sizeof(peak_time));
            for (int i = 0; i < num_peaks-1; i++){
                peak_time[i] = (float)(peaks[i + 1] - peaks[i]);
            }
            sum_peak_time = 0;
            for (int j = 0; j < num_peaks-1; j++)
                sum_peak_time += peak_time[j];
            memset(error_find, '\0', sizeof(error_find));
            error_find_len = 0;
            for (int j = 0; j < num_peaks-1; j++) {
                float  pt2 = (sum_peak_time - peak_time[j]) / (float)(num_peaks - 2);
                if( abs(pt2 - peak_time[j]) < 0.6 * pt2){
                    error_find[error_find_len]=peak_time[j];
                    error_find_len++;
                }
            }
            sum_error_time =0;
            if (error_find_len != 0) {
                for(int m=0 ;m<error_find_len;m++)
                    sum_error_time = sum_error_time + error_find[m];
                heart_rate = 60.0 / (sum_error_time / (float)error_find_len);
                heart_rate =   heart_rate * 200; 
            }
            HR = (uint8_t)(roundf(heart_rate * 10) / 10);
            if(HR==0){
                HR = HR_T+1;

            }
            else if(HR==255){
                HR = HR_T+2;
                
            }else{
                HR_T = HR;
            }

        printf("心率2 = %d\n",HR);
            /****************************************************/
            memset(baseline, '\0', sizeof(baseline));
            memset(ppgir_ac, '\0', sizeof(ppgir_ac));
            medfilt(PPGIR ,1000 ,baseline);//提取IR基線
            for (int i = 0; i < 1000; i++)
                ppgir_ac[i] = (int16_t)(PPGIR[i] - baseline[i]);//去除IR基線
            memset(baseline, '\0', sizeof(baseline));
            memset(ppgred_ac, '\0', sizeof(ppgred_ac));
            medfilt(PPGRED ,1000 ,baseline);//提取RED基線
            for (int i = 0; i < 1000; i++)
                ppgred_ac[i] = (int16_t)(PPGRED[i] - baseline[i]);//去除RED基線
//           for(int i=0;i<1000;i++)
//              printf("%d (red=%d / RED=%d)/(ir=%d  / IR=%d) \n",i ,ppgred_ac[i],PPGRED[i] ,ppgir_ac[i],PPGIR[i]);
            memset(R, '\0', sizeof(R));
            printf("ss");
            MA=0;MB=0;MC=0;MD=0;
            for (int i = 0; i < 1000; i++){
                r1 = (int64_t)( ppgred_ac[i]*(int16_t)1000000 / PPGRED[i] ) ;
                r2 = (int64_t)( ppgir_ac[i]*(int16_t)1000000 / PPGIR[i] ) ;
                ss =r1/r2;
                MA =MA+ss;
                X1 = ((int64_t)-50060 *ss*ss) ;
                X1 = X1/1000;
                X2 = ((int64_t)30354 * ss) ;
                X2 = X2/1000;
                R[i] = X1 + X2 + 95;
                printf("%d ,",ss);
            }
            MB=MA/1000;
            MB=MB*0.045;
            MC=100-MB;
            printf("MC===============%d",MC);
            printf("\n\nR");
            memset(sp2o_errorpick, '\0', sizeof(sp2o_errorpick));
            errorpick_index=0;
            for (int i = 0; i < 1000; i++) {
                if (R[i] > 70 && R[i] <= 100) {
                    sp2o_errorpick[errorpick_index] = R[i];
                    errorpick_index++;
                }
            }
            sp2o_errorpick_sum=0;
            for(int i=0;i<errorpick_index;i++)
                sp2o_errorpick_sum = sp2o_errorpick_sum + sp2o_errorpick[i];
            SP2 = ((int)sp2o_errorpick_sum / errorpick_index);
            SP2=SP2+4;
            SP2=(int)MC;
            if(SP2>99)
             SP2=99;
        printf("血氧=%d\n",SP2);
            
            /****************************************************/
            
//int32_t ppgir_step1[1000] = {0};
//int32_t ppgir_step2[1000] = {0};
//double ppgir_step3[1000] = {0};
//double ppgir_ml[1000] = {0};
//int tlmin[300]={0};
//int tlmax[300]={0};
//int32_t ppgmax=0;
//int tlmin_count=0;
//int tlmax_count=0;
//int bp5=0;
//int sut[150]={0};
//int cp[150]={0};
//int dt[150]={0};
//int msut=0;
//int mcp=0;
//int mdt=0;
//int sc,dc,cc=0;
//int mdt1,mdt2,mcp1=0;

            tlmax_count=0;//int
            tlmin_count=0;//int
            memset(ppgir_step1, '\0', sizeof(ppgir_step1));
            ppgmax=0;
            for (int i=0;i<1000;i++){
                ppgir_step1[i]=(int32_t)(PPGIR[i]*(-1));
                if (PPGIR[i]>ppgmax)
                    ppgmax= (int32_t)PPGIR[i];             
            }
//                                                                                    printf("ppgir_step1=\n\n");
//                                                                                    for (int i=0;i<1000;i++)
//                                                                                       printf("%d ",ppgir_step1[i]);
//                                                                                    printf("\n");
            for (int i=0;i<1000;i++){
                  ppgir_step1[i]=ppgir_step1[i]+ppgmax;                
            }
//                                                                                    printf("ppgir_step1=\n\n");
//                                                                                    for (int i=0;i<1000;i++)
//                                                                                        printf("%d ",ppgir_step1[i]);
//                                                                                    printf("\n");
//                                                                                    printf("ppgmax=%d\n",ppgmax);
            memset(ppgir_step2, '\0', sizeof(ppgir_step2));
            moving_average_ml(ppgir_step1, 1000 ,4 ,ppgir_step2);
//                                                                                    printf("ppgir_step2=\n\n");
//                                                                                    for (int i=0;i<1000;i++)
//                                                                                        printf("%d ",ppgir_step2[i]);
//                                                                                    printf("\n");
        
            memset(ppgir_ml, '\0', sizeof(ppgir_ml));
            for(int i=0;i<1000;i++)
                ppgir_step3[i]=(double)ppgir_step2[i];

            //                                                                        printf("ff in\n");
            filtfilt(ppgir_step3,ppgir_ml);
            //                                                                        printf("ff out\n");
            //                                                                        printf("ppgir_ml=\n\n");
                                                                                    //for (int i=0;i<1000;i++)
                                                                                    //    printf("%d ",(int)(ppgir_ml[i]));
        
            memset(tlmin, '\0', sizeof(tlmin));
            tlmin_count=0;
            for (int i = 50; i < 950 ; i++) {
                if (ppgir_ml[i] < ppgir_ml[i-1] && ppgir_ml[i] < ppgir_ml[i+1]) {
                    tlmin[tlmin_count]=i;
                    tlmin_count++;
                }
            }

            //                                                                        printf("tlmin_count=%d\n",tlmin_count);
            memset(tlmax, '\0', sizeof(tlmax));
            tlmax_count=0;
            for (int j = 50; j <  950 ; j++) {
                if (ppgir_ml[j] > ppgir_ml[j-1] && ppgir_ml[j] > ppgir_ml[j+1]) {
                    tlmax[tlmax_count]=j;
                    tlmax_count++;
                }
            }
            //                                                                        printf("tlmax_count=%d\n",tlmax_count);

            if(tlmax_count>tlmin_count){
                bp5=tlmin_count/2;
            }
            else{
                bp5=tlmax_count/2;    
            }
            //                                                                        printf("bp5=%d\n",bp5);
            if (tlmax[0]>tlmin[0]){//max:even min:even
                if (ppgir_ml[tlmax[0]]-ppgir_ml[tlmin[0]]>ppgir_ml[tlmax[1]]-ppgir_ml[tlmin[1]]){
                    for(int i=0;i<bp5-1;i++){
                        sut[i]=tlmax[2*i]-tlmin[2*i];
                        dt[i]=tlmin[2*i+2]-tlmax[2*i];
                        cp[i]=tlmax[2*i+2]-tlmax[2*i];
                    }
                }
                else{//max:odd min:odd
                    for(int i=0;i<bp5-1;i++){
                        sut[i]=tlmax[2*i+1]-tlmin[2*i+1];
                        dt[i]=tlmin[2*i+3]-tlmax[2*i+1];
                        cp[i]=tlmax[2*i+3]-tlmax[2*i+1];
                    }
                }
            }
            else{
                if (ppgir_ml[tlmax[1]]-ppgir_ml[tlmin[0]]>ppgir_ml[tlmax[2]]-ppgir_ml[tlmin[1]]){//max:odd min:even
                    for(int i=0;i<bp5-1;i++){
                        sut[i]=tlmax[2*i+1]-tlmin[2*i];
                        dt[i]=tlmin[2*i+2]-tlmax[2*i+1];
                        cp[i]=tlmax[2*i+3]-tlmax[2*i+1];
                    }
                }
                else{//max:even min:odd
                    for(int i=0;i<bp5-1;i++){
                        sut[i]=tlmax[2*i+2]-tlmin[2*i+1];
                        dt[i]=tlmin[2*i+1]-tlmax[2*i];
                        cp[i]=tlmax[2*i+2]-tlmax[2*i];
                    }
                }
            }
            
            //                                                            for(int i=0;i<30;i++)
            //                                                                printf("%d tlmin=%d ,tlmax=%d \n",i,tlmin[i],tlmax[i]);
            //                                                           for(int i=0;i<bp5-1;i++)
            //                                                                printf("i=%d ,sut=%d ,dt=%d ,cp=%d\n",i,sut[i],dt[i],cp[i]);
            
            sc=0;dc=0;cc=0;//錯誤數
            msut=0;mdt=0;mcp=0;//平均數
            for (int i=0;i<bp5-1;i++){
                    msut=msut+sut[i];
                    mdt=mdt+dt[i];
                    mcp=mcp+cp[i];
            }
            msut=(int)(msut/(bp5-1));
            mdt=(int)(mdt/(bp5-1));
            mcp=(int)(mcp/(bp5-1));
            for (int i=0;i<bp5-1;i++){
                 if(sut[i]*10>16*msut){
                    sut[i]=0;
                    sc++;
                }
                if(dt[i]*10>16*mdt){
                    dt[i]=0;
                    dc++;
                }
                if(cp[i]*10>16*mcp){
                   cp[i]=0;
                   cc++;
                 }
                 if(sut[i]*10<6*msut){
                    sut[i]=0;
                    sc++;
                }
                if(dt[i]*10<6*mdt){
                    dt[i]=0;
                    dc++;
                }
                if(cp[i]*10<6*mcp){
                   cp[i]=0;
                   cc++;
                 }                                 
            }
            msut=0;mdt=0;mcp=0;
            for (int i=0;i<bp5-1;i++){
                msut=msut+sut[i];
                mdt=mdt+dt[i];
                mcp=mcp+cp[i];
            }
            if(sc<bp5-1)
                msut=(int)(msut/(bp5-1-sc));
            if(dc<bp5-1)
                mdt=(int)(mdt/(bp5-1-dc));
            if(cc<bp5-1)
                mcp=(int)(mcp/(bp5-1-cc));
            memset(machine_learning, '\0', sizeof(machine_learning));
            mcp1=0.8*(double)mcp;
            mdt1=0.5*(double)mdt;
            mdt2=0.66*(double)mdt;

            machine_learning[0]= (uint8_t) (msut);
            machine_learning[1]= (uint8_t) (mcp);
            machine_learning[2]= (uint8_t) (mcp1);
            machine_learning[3]= (uint8_t) (mdt);
            machine_learning[4]= (uint8_t) (mdt1);
            machine_learning[5]= (uint8_t) (mdt2);

            printf("[");
            for (int i = 0; i < 6; i++) 
                printf("%d ", machine_learning[i]);
            printf("]\n");

            msut=msut*33;
            msut=(int)(msut/33);

            mcp=mcp*33;
            mcp=(int)(mcp/175);

            mcp1=mcp1*33;
            mcp1=(double)(mcp1/175);

            mdt=mdt*33;
            mdt=(int)(mdt/153);

            mdt1=mdt1*33;
            mdt1=(double)(mdt1/153);

            mdt2=mdt2*33;
            mdt2=(double)(mdt2/153);


            machine_learning[0]= (uint8_t) (msut);
            machine_learning[1]= (uint8_t) (mcp);
            machine_learning[2]= (uint8_t) (mcp1);
            machine_learning[3]= (uint8_t) (mdt);
            machine_learning[4]= (uint8_t) (mdt1);
            machine_learning[5]= (uint8_t) (mdt2);
            
            printf("[");
            for (int i = 0; i < 6; i++) 
                printf("%d ", machine_learning[i]);
            printf("]\n");

            /****************/
	        Tmp117_GetTempC(&BodTemp);
	    //    printf("BodyTemp : %u.%d\n\r", BodTemp/10 ,BodTemp%10);
            /****************/
            tflitemicro_algo_init();
            result_t test_result = tflitemicro_algo_run(&machine_learning[0]);
            printf("1-BP0=%d BP1=%d\n",test_result.BP0,test_result.BP1);
            BPU=(int)(2*test_result.BP0);
            BPD=(int)(2*test_result.BP1);
            BPU=abs(BPU);
            BPD=abs(BPD);
            BPU=BPU+2*BPD;
            BPD=BPU+(int)(428/(int)HR)-55;
			St7735_Display(Lv, SOC, HR, BodTemp, SP2,abs(BPU),abs(BPD));
        //    printf("2-BP0=%d BP1=%d\n",abs(BPU),abs(BPD));
            /****************/
            
//int hr_kk[6]={0};
//int hr_n=0;
//int hr_flag=0;
//int sp2o_kk[6]={0};
//int sp2o_n=0;
//int sp2o_flag=0;

            if(hr_flag==0){
                hr_kk[0]=hr_kk[1];
                hr_kk[1]=hr_kk[2];
                hr_kk[2]=hr_kk[3];
                hr_kk[3]=hr_kk[4];
                hr_kk[4]=hr_kk[5];
                hr_kk[5]=(int)HR;
                for(int i=0;i<6;i++){
                    if(hr_kk[i] >140)
                        hr_n++;
                }
                if(hr_n!=6){
                    hr_n=0;
                }
                else if(hr_n==6){
                    hr_n=0;
                    hr_flag=1;
                }
            }else{
                hr_kk[0]=hr_kk[1];
                hr_kk[1]=hr_kk[2];
                hr_kk[2]=(int)HR;
                for(int i=0;i<6;i++){
                    if(hr_kk[i] <140)
                        hr_n++;
                }
                if(hr_n!=6){
                    hr_n=0;
                }
                else{
                    hr_n=0;
                    hr_flag=0;
                }
                Sd178b_Call(1);
            }
            /*
            if(sp2o_flag==0){
                sp2o_kk[0]=sp2o_kk[1];
                sp2o_kk[1]=sp2o_kk[2];
                sp2o_kk[2]=sp2o_kk[3];
                sp2o_kk[3]=sp2o_kk[4];
                sp2o_kk[4]=sp2o_kk[5];
                sp2o_kk[5]=SP2;
                for(int i=0;i<6;i++){
                    if(sp2o_kk >140)
                        sp2o_n++;
                }
                if(hr_n!=6){
                    sp2o_n=0;
                }
                else{
                    sp2o_n=0;
                    sp2o_flag=1;
                }
            }else{
                sp2o_kk[0]=sp2o_kk[1];
                sp2o_kk[1]=sp2o_kk[2];
                sp2o_kk[2]=SP2;
                for(int i=0;i<6;i++){
                    if(sp2o_kk <140)
                        sp2o_n++;
                }
                if(hr_n!=6){
                    sp2o_n=0;
                }
                else{
                    sp2o_n=0;
                    sp2o_flag=0;
                }
                Sd178b_Call();
            }
            */
            /****************/
            char uart0_buf[68] = "BLE3011aaabbbcccdddeeefff022011094362097116079034000002347051004002";
            char backup[68 - 22 + 1]; // 22 是需要修改的部分起始位置
            strcpy(backup, &uart0_buf[25]);

            sprintf(&uart0_buf[6], "%01d", Lv);//等級
            sprintf(&uart0_buf[7], "%03d", SOC);//呼吸
            sprintf(&uart0_buf[10], "%03d", HR);//心跳
            sprintf(&uart0_buf[13], "%03d", (int)BodTemp);//體溫
            sprintf(&uart0_buf[16], "%03d", SP2);//血氧
            sprintf(&uart0_buf[19], "%03d", BPU);//收縮
            sprintf(&uart0_buf[22], "%03d", BPD);//舒張

            strcpy(&uart0_buf[25], backup);
            uart0_ptr->uart_write(uart0_buf, strlen(uart0_buf));
            board_delay_ms(10);
        }
    printf("END\n");
    }
}