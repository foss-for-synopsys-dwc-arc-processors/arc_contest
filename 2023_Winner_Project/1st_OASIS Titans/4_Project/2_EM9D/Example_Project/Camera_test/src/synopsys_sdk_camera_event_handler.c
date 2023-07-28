#include "synopsys_sdk_camera_event_handler.h"

#define DBG_APP_PRINT_LEVEL 

//datapath callback flag
extern uint8_t g_xdma_abnormal;
extern uint8_t g_rs_abnormal;
extern uint8_t g_hog_abnormal;
extern uint8_t g_rs_frameready;
extern uint8_t g_hog_frameready;
extern uint8_t g_md_detect;
extern uint8_t g_cdm_fifoerror;
extern uint8_t g_wdt1_timeout;
extern uint8_t g_wdt2_timeout;
extern uint8_t g_wdt3_timeout;
extern int32_t g_inp1bitparer_abnormal;
extern uint32_t g_dp_event;
extern uint8_t g_frame_ready;

extern uint32_t g_cur_hw5x5jpeg_frame;
extern uint32_t g_hw5x5jpeg_acc_frame;

extern uint32_t g_wdma2_baseaddr;

void synopsys_camera_evt_handler(EVT_INDEX_E event)
{
    uint8_t human_present = 0;
    uint16_t err;
    g_dp_event = event;

    xprintf("Interrupt In: %2d \n", event);
    switch (event)
    {
    case EVT_INDEX_1BITPARSER_ERR: /*reg_inpparser_fs_cycle_error*/
        hx_drv_inp1bitparser_get_errstatus(&err);
        printf("err=0x%x\n", err);
        hx_drv_inp1bitparser_clear_int();
        hx_drv_inp1bitparser_set_enable(0);
        g_inp1bitparer_abnormal = 1;
        break;
    case EVT_INDEX_EDM_WDT1_TIMEOUT:
        g_wdt1_timeout = 1;
        break;
    case EVT_INDEX_EDM_WDT2_TIMEOUT:
        g_wdt2_timeout = 1;
        break;
    case EVT_INDEX_EDM_WDT3_TIMEOUT:
        g_wdt3_timeout = 1;
        break;

    case EVT_INDEX_CDM_FIFO_ERR:
        /*
                * error happen need CDM timing & TPG setting
                * 1. SWRESET Datapath
                * 2. restart streaming flow
                */
        g_cdm_fifoerror = 1;

        break;

    case EVT_INDEX_XDMA_WDMA1_ABNORMAL:
    case EVT_INDEX_XDMA_WDMA2_ABNORMAL:
    case EVT_INDEX_XDMA_WDMA3_ABNORMAL:
    case EVT_INDEX_XDMA_RDMA_ABNORMAL:
        /*
                * error happen need
                * 1. SWRESET Datapath
                * 2. restart streaming flow
                */
        g_xdma_abnormal = 1;
        break;

    case EVT_INDEX_RSDMA_ABNORMAL:
        /*
                * error happen need
                * 1. SWRESET RS & RS DMA
                * 2. Re-run flow again
                */
        g_rs_abnormal = 1;
        break;

    case EVT_INDEX_HOGDMA_ABNORMAL:
        /*
                * error happen need
                * 1. SWRESET HOG & HOG DMA
                * 2. Re-run flow again
                */
        g_hog_abnormal = 1;
        break;

    case EVT_INDEX_CDM_MOTION:
        /*
                * app anything want to do
                * */
        g_md_detect = 1;
        break;
    case EVT_INDEX_XDMA_FRAME_READY:
        g_cur_hw5x5jpeg_frame++;
        g_hw5x5jpeg_acc_frame++;

        g_frame_ready = 1;
        break;
    case EVT_INDEX_RSDMA_FINISH:
        g_rs_frameready = 1;
        break;
    case EVT_INDEX_HOGDMA_FINISH:
        g_hog_frameready = 1;
        break;
    case EVT_INDEX_SENSOR_RTC_FIRE:
        break;
    default:
        printf("Other Event %d\n", event);
        break;
    }


    printf("app_aiot_face_hw5x5jpeg_eventhdl_cb \n");
    cis_raw_algo_check(g_wdma2_baseaddr);
}

