#include "main.h"
#include "tflitemicro_algo.h"
#include "model_settings.h"
#include "aud_lib.h"
#include "hx_drv_iomux.h"
#include <time.h>

#define SERIAL_SEND_AUDIO
//#define USE_UART_PRINT
#if defined(USE_UART_PRINT) || defined(SERIAL_SEND_AUDIO)
DEV_UART * uart0_ptr = NULL;
#endif

#ifdef USE_UART_PRINT
char uart_buf[uart_buf_size] = {0};
#define uprint(format, ...) do { \
    sprintf(uart_buf, format, ## __VA_ARGS__); \
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf)); \
    board_delay_ms(5); \
} while(0);
#define print uprint
#else
#include <stdio.h>
#define print printf
#endif

float input_buf[kImageSize] = {0};

#define mic_sample_rate 16000
#define AUD_BLK_STEP 4
#define AUD_WAIT_STEP 0
#define AUD_BLK_SZ 512	//0.064sx
#define AUD_BLK_SZ_SHIFT 9
#define AUD_BLK_STEP_SZ (AUD_BLK_SZ * AUD_BLK_STEP)	//0.256s = 0.064 * 4
#define AUD_STEP_CNT 1	//Record time: X * 0.256s
#define AUD_BUF_SIZE (AUD_BLK_STEP_SZ * AUD_STEP_CNT)
#define SEND_AUD_BUF_SIZE (mic_sample_rate)

#if AUD_BUF_SIZE < kImageSize
#error "AUD_BUF_SIZE is smaller than kImageSize."
#endif

typedef struct {
    int16_t left;
    int16_t right;
} META_AUDIO_t;

volatile META_AUDIO_t audio_clip_stamp[AUD_BLK_STEP_SZ] = {0};
volatile META_AUDIO_t audio_clip[AUD_BUF_SIZE] = {0};
static META_AUDIO_t send_audio_clip[SEND_AUD_BUF_SIZE];
int audio_flag = 0;
int send_audio_idx = 0;

audio_config_t aud_pdm_cfg;

void pdm_rx_callback_fun(uint32_t status);
void mic_init();
int capture_data_start();
void capture_data_process();
void capture_data();
static int send_audio_to_cloud(META_AUDIO_t *audio, size_t audio_size);

void LED_init();
int green_flag = 0;

int main(void)
{
    const int mic_st_pos = 0;
    int predict_idx = 0, predict_cnt = 0, skip_cnt = 0, skip_cap_aud = 0;
    int idx, res[12] = {0};
    int i, j;

#if defined(USE_UART_PRINT) || defined(SERIAL_SEND_AUDIO)
    //UART 0 is already initialized with 115200bps
    uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);
#endif

    mic_init();
    print("Microphone Initialize\r\n");

    LED_init();
    print("LED Initialize\r\n");

    print("TFLM external stream Start\r\n");
    int tflm_init_result = 0;
    tflm_init_result = tflitemicro_algo_init();
    if (tflm_init_result == -2) {
        return tflm_init_result;
    }
    print("TFLM init done\r\n");

    capture_data();

    while (1)
    {
        capture_data_start();
        for (i = 0; i < AUD_BUF_SIZE/kImageSize; ++i)
        {
            for (j = 0; j < kImageSize; ++j)
            {
                input_buf[j] = (audio_clip[mic_st_pos + i*kImageSize+j].left + audio_clip[mic_st_pos + i*kImageSize+j].right) / 65536.0;
            }
            idx = tflitemicro_algo_run(input_buf);

            if (predict_idx == idx && skip_cnt <= 0) {
                ++predict_cnt;
                if (predict_cnt > 30 && idx == M_CATEGORY_HAPPY) {
                    capture_data_process();
                    skip_cap_aud = 1;
                    skip_cnt = 20;
                    print(" -- res [%s], cnt %d --\r\n", kCategoryLabels[idx], predict_cnt);
                    send_audio_to_cloud(send_audio_clip, SEND_AUD_BUF_SIZE);
                }
            }
            else {
                predict_idx = idx;
                predict_cnt = 0;
            }
            --skip_cnt;
            if (skip_cnt > 0)
                print("skip_cnt %d\r\n", skip_cnt);
            else {
                if (green_flag) {
                    hx_drv_iomux_set_outvalue(IOMUX_PGPIO12, 0);
                    green_flag = 0;
                }
            }
        }

        if (SEND_AUD_BUF_SIZE > send_audio_idx + AUD_BUF_SIZE) {
            memcpy(send_audio_clip + send_audio_idx, (void*)audio_clip, sizeof(META_AUDIO_t)*AUD_BUF_SIZE);
            send_audio_idx += AUD_BUF_SIZE;
        }
        else {
            send_audio_idx = SEND_AUD_BUF_SIZE - AUD_BUF_SIZE;
            memcpy(send_audio_clip, send_audio_clip + AUD_BUF_SIZE, sizeof(META_AUDIO_t)*send_audio_idx);
            memcpy(send_audio_clip + send_audio_idx, (void*)audio_clip, sizeof(META_AUDIO_t)*AUD_BUF_SIZE);
        }

        if (skip_cap_aud)
            skip_cap_aud = 0;
        else
            capture_data_process();
    }

    return 0;
}

void pdm_rx_callback_fun(uint32_t status)
{
    static uint32_t last_block = 0;
    static uint32_t stamp_idx = 0;
    static uint32_t pdm_idx = 0;
    static uint8_t first_flag = 0;

    uint32_t pdm_buf_addr;
    uint32_t block;
    uint32_t data_size;
    uint32_t audio_size;

    hx_lib_audio_request_read(&pdm_buf_addr, &block);

    if(last_block > block)
    {
        audio_size = ((AUD_BLK_STEP + AUD_WAIT_STEP) - last_block) << AUD_BLK_SZ_SHIFT;
        data_size = audio_size << 2;

        memcpy((void *) (audio_clip_stamp + stamp_idx), (void *) (pdm_buf_addr + pdm_idx), data_size);

        pdm_idx = 0;
        stamp_idx = 0;
        last_block = 0;

        audio_flag = 1;
    }
    else
    {
        audio_size = (block - last_block) << AUD_BLK_SZ_SHIFT;
        data_size = audio_size << 2;

        memcpy((void *) (audio_clip_stamp + stamp_idx), (void *) (pdm_buf_addr + pdm_idx), data_size);

        stamp_idx = stamp_idx + audio_size;
        pdm_idx = pdm_idx + data_size;
        last_block = block;
    }
    return;
}

void mic_init()
{
    hx_lib_audio_set_if(AUDIO_IF_PDM);
    hx_lib_audio_init();
    audio_flag = 0;

    hx_lib_audio_register_evt_cb(pdm_rx_callback_fun);

    aud_pdm_cfg.sample_rate = AUDIO_SR_16KHZ;
    aud_pdm_cfg.buffer_addr = (uint32_t *) (0x20000000+36*1024);//0x20009000;
    aud_pdm_cfg.block_num = (AUD_BLK_STEP + AUD_WAIT_STEP);
    aud_pdm_cfg.block_sz = AUD_BLK_SZ * 4;
    aud_pdm_cfg.cb_evt_blk = 2;
}

int capture_data_start()
{
    return hx_lib_audio_start(&aud_pdm_cfg); /*Configure PDM specification*/
}

void capture_data_process()
{
    for (uint16_t record_step_cnt = 0; record_step_cnt < AUD_STEP_CNT; record_step_cnt++)
    {
        while(audio_flag == 0)
        {
            board_delay_ms(1);
        }
        audio_flag = 0;

        memcpy((void *)(audio_clip + (record_step_cnt * AUD_BLK_STEP_SZ)), (void *)(audio_clip_stamp), (AUD_BLK_STEP_SZ * 4));
    }
        hx_lib_audio_stop();
    return;
}

void capture_data()
{
    if(capture_data_start() != 0)
    {
        print("hx_lib_audio_start fail\r\n");
    }
    else
    {
        capture_data_process();
    }

    return;
}

void LED_init() {
    hx_drv_iomux_init();
    hx_drv_iomux_set_pmux(IOMUX_PGPIO12, 3); // Green LED Output; off
    hx_drv_iomux_set_outvalue(IOMUX_PGPIO12, 0);
    hx_drv_iomux_set_pmux(IOMUX_PGPIO9, 3); // Blue LED Output; off
    hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 0);
    hx_drv_iomux_set_pmux(IOMUX_PGPIO8, 3); // Red LED Output; on
    hx_drv_iomux_set_outvalue(IOMUX_PGPIO8, 0);
}

#if defined(SERIAL_SEND_AUDIO)
static int send_audio_to_cloud(META_AUDIO_t *audio, size_t audio_size)
{
    hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 1); // turn on Blue
    uint8_t user_key = 0;
    uint8_t function_ret;
    int is_pass = 0, i;
    clock_t st_t;

    printf("-start-\n");
    for (i = 0; i < audio_size; ++i) {
        printf("%d\n", (audio[i].left + audio[i].right)>>1);
    }
    printf("-end-\n");
    printf("-end-\n");

    st_t = clock();
    do {
        function_ret = uart0_ptr->uart_read_nonblock(&user_key, 1);
        if(function_ret != 1)
            user_key = 0;

        if(user_key == 'p') {
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 0); // turn off Blue
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO12, 1); // turn on Green
            green_flag = 1;
            user_key = 0;
            printf("PASS\n\n");
            is_pass = 1;
            break;
        }
        else if(user_key == 'f') {
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 0); // turn off Blue
            user_key = 0;
            printf("FAIL\n\n");
            is_pass = 0;
            break;
        }
    } while(clock() - st_t < 3*CLOCKS_PER_SEC);

    return is_pass;
}
#endif
