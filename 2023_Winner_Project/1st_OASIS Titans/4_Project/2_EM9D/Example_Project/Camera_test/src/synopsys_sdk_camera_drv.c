#include "synopsys_sdk_camera_drv.h"


#define DBG_APP_PRINT_LEVEL  DBG_MORE_INFO


extern WE1AppCfg_CustTable_t app_cust_config;
extern WE1AlgoCfg_CustTable_t algo_cust_config;

//datapath callback flag
volatile uint8_t g_xdma_abnormal = 0;
volatile uint8_t g_rs_abnormal = 0;
volatile uint8_t g_hog_abnormal = 0;
volatile uint8_t g_rs_frameready = 0;
volatile uint8_t g_hog_frameready = 0;
volatile uint8_t g_md_detect = 0;
volatile uint8_t g_cdm_fifoerror = 0;
volatile uint8_t g_wdt1_timeout = 0;
volatile uint8_t g_wdt2_timeout = 0;
volatile uint8_t g_wdt3_timeout = 0;
volatile int32_t g_inp1bitparer_abnormal = 0;
volatile uint32_t g_dp_event = 0;
volatile uint8_t g_frame_ready = 0;

//HW5x5 +JPEG Enc Frame
volatile uint32_t g_cur_hw5x5jpeg_frame = 0;
volatile uint32_t g_hw5x5jpeg_acc_frame = 0;

uint8_t g_jpeg_total_slot = 0;

CIS_XHSHUTDOWN_INDEX_E g_xshutdown_pin;

//tick cal
volatile uint32_t g_tick_start = 0, g_tick_stop = 0, g_tick_toggle = 0;
volatile uint32_t g_tick_period, g_period;
volatile uint32_t g_tick_sensor_std = 0, g_tick_sensor_stream = 0, g_tick_sensor_toggle = 0;

//error retry time
volatile uint32_t g_hw5x5jpeg_err_retry_cnt = 0;

uint32_t g_image_size = 0;
uint32_t g_wdma1_baseaddr = WE1_DP_WDMA1_OUT_SRAM_ADDR;
uint32_t g_wdma2_baseaddr = WE1_DP_WDMA2_OUT_SRAM_ADDR;
uint32_t g_wdma3_baseaddr = WE1_DP_WDMA3_OUT_SRAM_ADDR;
uint32_t g_jpegautofill_addr = WE1_DP_JPEG_HWAUTOFILL_SRAM_ADDR;
SENSORDPLIB_STREAM_E g_stream_type = SENSORDPLIB_STREAM_NONEAOS;
SENSORDPLIB_HM11B1_HEADER_T info;

void app_cpu_sleep_at_capture (void);

void app_1bitparser_err_info();
void app_set_dp_mclk_src();
void app_load_cfg_from_flash();

void app_stop_cv();
void app_init_dpcbflag();
void app_iot_facedetect_systemreset();

/**
 * \brief	application flag initial
 *
 * \return	void.
 */
void app_init_flag();

//void app_get_gpio(WE1AppCfg_GPIO_e app_gpio, IOMUX_INDEX_E *gpio_mux);
void app_get_xshutdown(WE1AppCfg_GPIO_e app_gpio, CIS_XHSHUTDOWN_INDEX_E *gpio_mux);
void app_print_cfg();

static int open_spi()
{
	int ret ;
#ifndef SPI_MASTER_SEND
	ret = hx_drv_spi_slv_open();
	dbg_printf(DBG_LESS_INFO, "SPI slave ");
#else
	ret = hx_drv_spi_mst_open();
	dbg_printf(DBG_LESS_INFO, "SPI master ");
#endif
    return ret;
}

// Config: WE1AppCfg_DP_CLK_Mux_XTAL, maybe can cut down
void app_set_dp_mclk_src()
{
    if (app_cust_config.we1_driver_cfg.dp_clk_mux == WE1AppCfg_DP_CLK_Mux_RC36M)
    {
        hx_drv_dp_set_dp_clk_src(DP_CLK_SRC_RC_36M);
    }
    else if (app_cust_config.we1_driver_cfg.dp_clk_mux == WE1AppCfg_DP_CLK_Mux_XTAL)
    {
        hx_drv_dp_set_dp_clk_src(DP_CLK_SRC_XTAL_24M_POST);
    }
    if (app_cust_config.we1_driver_cfg.mclk_clk_mux == WE1AppCfg_MCLK_CLK_Mux_XTAL)
    {
        hx_drv_dp_set_mclk_src(DP_MCLK_SRC_INTERNAL, DP_MCLK_SRC_INT_SEL_XTAL);
    }
    else if (app_cust_config.we1_driver_cfg.mclk_clk_mux == WE1AppCfg_MCLK_CLK_Mux_RC36M)
    {
        hx_drv_dp_set_mclk_src(DP_MCLK_SRC_INTERNAL, DP_MCLK_SRC_INT_SEL_RC36M);
    }
}


/*HM11B1 used only*/
void app_1bitparser_err_info()
{
    uint32_t de0_count;

    /*get inp1bitparser fsm*/
    hx_drv_inp1bitparser_get_fsm(&info.fsm);
    /*get inp1bitparser HW hvsize*/
    hx_drv_inp1bitparser_get_HW_hvsize(&info.hw_hsize, &info.hw_vsize);
    /*get inp1bitparser hvsize*/
    hx_drv_inp1bitparser_get_hvsize(&info.sensor_hsize, &info.sensor_vsize);
    /*get inp1bitparser frame len, line len*/
    hx_drv_inp1bitparser_get_framelinelen(&info.frame_len, &info.line_len);
    /*get inp1bitparser again*/
    hx_drv_inp1bitparser_get_again(&info.again);
    /*get inp1bitparser dgain*/
    hx_drv_inp1bitparser_get_dgain(&info.dgain);
    /*get inp1bitparser integration time*/
    hx_drv_inp1bitparser_get_intg(&info.intg);
    /*get inp1bitparser interrupt src*/
    hx_drv_inp1bitparser_get_intsrc(&info.intsrc);
    /*get inp1bitparser fstus*/
    hx_drv_inp1bitparser_get_fstus(&info.fstus);
    /*get inp1bitparser fc*/
    hx_drv_inp1bitparser_get_fc(&info.fc);
    /*get inp1bitparser crc*/
    hx_drv_inp1bitparser_get_crc(&info.sensor_crc, &info.hw_crc);
    hx_drv_inp1bitparser_get_cycle(&info.fs_cycle, &info.fe_cycle);
    hx_drv_inp1bitparser_get_fscycle_err_cnt(&info.fs_cycle_err_cnt);
    hx_drv_inp1bitparser_get_errstatus(&info.err_status);

    dbg_printf(DBG_LESS_INFO, "fsm=%d\n", info.fsm);
    dbg_printf(DBG_LESS_INFO, "hw_hsize=%d,hw_vsize=%d\n", info.hw_hsize, info.hw_vsize);
    dbg_printf(DBG_LESS_INFO, "sensor_hsize=%d,sensor_vsize=%d\n", info.sensor_hsize, info.sensor_vsize);
    dbg_printf(DBG_LESS_INFO, "sensor_crc=0x%x,hw_crc=0x%x\n", info.sensor_crc, info.hw_crc);
    dbg_printf(DBG_LESS_INFO, "fs_cycle=%d,fe_cycle=%d\n", info.fs_cycle, info.fe_cycle);
    dbg_printf(DBG_LESS_INFO, "fs_cycle_err_cnt=%d\n", info.fs_cycle_err_cnt);
    dbg_printf(DBG_LESS_INFO, "err_status=%d\n", info.err_status);

    hx_drv_inp1bitparser_clear_int();
    hx_drv_inp1bitparser_set_enable(0);
    hx_drv_edm_get_de_count(0, &de0_count);
    dbg_printf(DBG_LESS_INFO, "de0_count=%d\n", de0_count);

    sensordplib_stop_capture();
    if (app_sensor_standby() != 0)
    {
        dbg_printf(DBG_LESS_INFO, "standby sensor fail 9\n");
    }
    sensordplib_start_swreset();
    sensordplib_stop_swreset_WoSensorCtrl();
    g_inp1bitparer_abnormal = 0;
    app_sensor_xshutdown_toggle();
    if (app_config_sensor_WE1_rx(1, SENSOR_STROBE_REQ) != 0)
    {
        dbg_printf(DBG_LESS_INFO, "re-setup sensor fail\n");
    }
}

void app_load_cfg_from_flash()
{
    uint32_t apptotal_len = sizeof(app_cust_config);
    uint32_t appcfg_len = 0;
    uint32_t flash_appcfg_addr = FLASH_APPCFG_BASEADDR;
    uint32_t appcfg_sram_addr = (uint32_t)&app_cust_config;
    uint32_t appcfg_content_sram_addr = 0;
    uint32_t appcfg_header_size = 0;
    uint8_t pu8CRC8_appcfg = 0;
    uint16_t pu16CheckSum_appcfg = 0;

    uint32_t algototal_len = sizeof(algo_cust_config);
    uint32_t algocfg_len = 0;
    uint32_t flash_algocfg_addr = FLASH_ALGOCFG_BASEADDR;
    uint32_t algocfg_sram_addr = (uint32_t)&algo_cust_config;
    uint32_t algocfg_content_sram_addr = 0;
    uint32_t algocfg_header_size = 0;
    uint8_t pu8CRC8_algocfg = 0;
    uint16_t pu16CheckSum_algocfg = 0;

    appcfg_header_size += sizeof(app_cust_config.table_header);                /**< Table Header */
    appcfg_header_size += sizeof(app_cust_config.app_table_info);              /**< Application Table information */
    appcfg_header_size += sizeof(app_cust_config.sensor_cfg_table_info);       /**< Sensor Configuration Table information */
    appcfg_header_size += sizeof(app_cust_config.sensor_streamon_table_info);  /**< Sensor Stream On Table information */
    appcfg_header_size += sizeof(app_cust_config.sensor_streamoff_table_info); /**< Sensor Stream Off Table information */
    appcfg_header_size += sizeof(app_cust_config.we1_table_info);              /**< WE-1 Driver Table information */
    appcfg_header_size += sizeof(app_cust_config.soc_com_table_info);          /**< SOC Communication Table information */

    appcfg_len = apptotal_len - appcfg_header_size;
    appcfg_content_sram_addr = appcfg_sram_addr + appcfg_header_size;
    dbg_printf(DBG_LESS_INFO, "header_size=0x%x,apptotal_len=%d,appcfg_len=%d,appcfg_sram_addr=0x%x,AppSRAMContent=0x%x\n", appcfg_header_size, apptotal_len, appcfg_len, appcfg_sram_addr, appcfg_content_sram_addr);

    if (hx_drv_spi_flash_protocol_read(0, flash_appcfg_addr, appcfg_sram_addr, appcfg_len, 4) != 0)
    {
        dbg_printf(DBG_LESS_INFO, "read flash fail flash_appcfg_addr=0x%x,appcfg_sram_addr=0x%x,len=%d\n", flash_appcfg_addr, appcfg_sram_addr, appcfg_len);
        app_iot_table_def_config();
    }
    else
    {
        dbg_printf(DBG_LESS_INFO, "read flash success flash_appcfg_addr=0x%x,appcfg_sram_addr=0x%x,len=%d\n", flash_appcfg_addr, appcfg_sram_addr, appcfg_len);
        HxGetCRC8((uint8_t *)appcfg_content_sram_addr, appcfg_len, &pu8CRC8_appcfg, &pu16CheckSum_appcfg);
        dbg_printf(DBG_LESS_INFO, "pu8CRC8_appcfg=0x%x,pu16CheckSum=0x%x\n", pu8CRC8_appcfg, pu16CheckSum_appcfg);

        if ((app_cust_config.table_header.table_crc != pu8CRC8_appcfg) || (app_cust_config.table_header.table_checksum != pu16CheckSum_appcfg) || (app_cust_config.table_header.table_version != APP_CONFIG_TABLE_VERSION))
        {
            dbg_printf(DBG_LESS_INFO, "App CFG bin have problem load default\n");
            dbg_printf(DBG_LESS_INFO, "app_table_crc=0x%x, cal_appcrc=0x%x\n", app_cust_config.table_header.table_crc, pu8CRC8_appcfg);
            dbg_printf(DBG_LESS_INFO, "app_table_chksum=0x%x, cal_appchksum=0x%x\n", app_cust_config.table_header.table_checksum, pu16CheckSum_appcfg);
            dbg_printf(DBG_LESS_INFO, "app_table_version=0x%x, app_h_version=0x%x\n", app_cust_config.table_header.table_version, APP_CONFIG_TABLE_VERSION);
            app_iot_table_def_config();
        }
    }

    algocfg_header_size += sizeof(algo_cust_config.table_header);    /**< Table Header */
    algocfg_header_size += sizeof(algo_cust_config.algo_table_info); /**< Application Table information */
    algocfg_len = algototal_len - algocfg_header_size;
    algocfg_content_sram_addr = algocfg_sram_addr + algocfg_header_size;
    dbg_printf(DBG_LESS_INFO, "algoheader_size=0x%x,algototal_len=%d,algocfg_len=%d,algocfg_sram_addr=0x%x,AlgoSRAMContent=0x%x\n", algocfg_header_size, algototal_len, algocfg_len, algocfg_sram_addr, algocfg_content_sram_addr);

    if (hx_drv_spi_flash_protocol_read(0, flash_algocfg_addr, algocfg_sram_addr, algocfg_len, 4) != 0)
    {
        dbg_printf(DBG_LESS_INFO, "read flash fail flash_algocfg_addr=0x%x,algocfg_sram_addr=0x%x,len=%d\n", flash_algocfg_addr, algocfg_sram_addr, algocfg_len);
        app_algo_table_def_config();
    }
    else
    {
        dbg_printf(DBG_LESS_INFO, "read flash success flash_algocfg_addr=0x%x,algocfg_sram_addr=0x%x,len=%d\n", flash_algocfg_addr, algocfg_sram_addr, algocfg_len);
        HxGetCRC8((uint8_t *)algocfg_content_sram_addr, algocfg_len, &pu8CRC8_algocfg, &pu16CheckSum_algocfg);
        dbg_printf(DBG_LESS_INFO, "pu8CRC8_algocfg=0x%x,pu16CheckSum_algocfg=0x%x\n", pu8CRC8_algocfg, pu16CheckSum_algocfg);
        if ((algo_cust_config.table_header.table_crc != pu8CRC8_algocfg) || (algo_cust_config.table_header.table_checksum != pu16CheckSum_algocfg) || (algo_cust_config.table_header.table_version != ALGO_CONFIG_TABLE_VERSION))
        {
            dbg_printf(DBG_LESS_INFO, "Algo CFG bin have problem load default\n");
            dbg_printf(DBG_LESS_INFO, "algo_table_crc=0x%x, cal_appcrc=0x%x\n", algo_cust_config.table_header.table_crc, pu8CRC8_algocfg);
            dbg_printf(DBG_LESS_INFO, "algo_table_chksum=0x%x, cal_appchksum=0x%x\n", algo_cust_config.table_header.table_checksum, pu16CheckSum_algocfg);
            dbg_printf(DBG_LESS_INFO, "algo_table_version=0x%x, app_h_version=0x%x\n", algo_cust_config.table_header.table_version, ALGO_CONFIG_TABLE_VERSION);
            app_algo_table_def_config();
        }
    }
}

/*application init flag*/
void app_init_flag()
{
#ifdef SUPPORT_HW5X5_ONLY
    g_image_size = app_cust_config.we1_driver_cfg.hw5x5_cfg.hw55_in_width * app_cust_config.we1_driver_cfg.hw5x5_cfg.hw55_in_height;
#else
    g_image_size = app_cust_config.we1_driver_cfg.jpeg_cfg.enc_width * app_cust_config.we1_driver_cfg.jpeg_cfg.enc_height;
#endif

    app_init_dpcbflag();

    //JPEG Decoder
    g_jpeg_total_slot = 0;

    //HW5x5 JPEG Enc
    g_cur_hw5x5jpeg_frame = 0;

    //err count init
    g_hw5x5jpeg_err_retry_cnt = 0;


}

// Config: WE1AppCfg_GPIO_IOMUX_SGPIO1, maybe can cut down
void app_get_xshutdown(WE1AppCfg_GPIO_e app_gpio, CIS_XHSHUTDOWN_INDEX_E *gpio_mux)
{
    switch (app_gpio)
    {
    case WE1AppCfg_GPIO_IOMUX_SGPIO0:
        *gpio_mux = CIS_XHSUTDOWN_IOMUX_SGPIO0;
        break;
    case WE1AppCfg_GPIO_IOMUX_SGPIO1:
        *gpio_mux = CIS_XHSUTDOWN_IOMUX_SGPIO1;
        break;
    default:
        *gpio_mux = CIS_XHSUTDOWN_IOMUX_NONE;
        break;
    }
}



void synopsys_camera_init (void)
{
    uint8_t idx = 0;
    uint8_t sensor_init_required = 0;
    uint16_t sensor_id;
    uint8_t rev_id;

    arc_int_disable(BOARD_SYS_TIMER_INTNO); // TIMER0
    arc_int_disable(BOARD_STD_TIMER_INTNO); // TIMER1

    int ret = -1;
    app_load_cfg_from_flash();
    app_init_flag();
    app_set_dp_mclk_src();
    /*TODO read bootup reason*/

    /*TODO according to chip package and sensor ID configure share pin*/
    app_rx_set_wlcsp38_sharepin();
    app_get_xshutdown(app_cust_config.we1_driver_cfg.xshutdown_pin_sel, &g_xshutdown_pin);

    // dbg_printf(DBG_LESS_INFO, "[WEI FIRMWARE] Daemon Process\n\tEvent Handler");

    event_handler_init();

    sensordplib_start_swreset();
    sensordplib_stop_swreset_WoSensorCtrl();
    ret = open_spi();
    if (ret != 0)
    {
        dbg_printf(DBG_LESS_INFO, "initial fail\n");
    }
    else
    {
        dbg_printf(DBG_LESS_INFO, " initial done\n");
    }
    //acc flag
    g_hw5x5jpeg_acc_frame = 0;

    g_wdma1_baseaddr = app_cust_config.we1_driver_cfg.wdma1_startaddr;
    g_wdma2_baseaddr = app_cust_config.we1_driver_cfg.wdma2_startaddr;
    g_wdma3_baseaddr = app_cust_config.we1_driver_cfg.wdma3_startaddr;
    g_jpegautofill_addr = app_cust_config.we1_driver_cfg.jpegsize_autofill_startaddr;

    sensordplib_set_xDMA_baseaddrbyapp(g_wdma1_baseaddr, g_wdma2_baseaddr, g_wdma3_baseaddr);
    sensordplib_set_jpegfilesize_addrbyapp(g_jpegautofill_addr);
	//Power Plan
	hx_drv_pmu_set_ctrl(PMU_PWR_PLAN, app_cust_config.we1_driver_cfg.pmu_powerplan);

    /*Sensor Configuration*/
    sensor_init_required = 1;
    dbg_printf(DBG_LESS_INFO, "sensor_init_required=%d\n", sensor_init_required);
    if (sensor_init_required == 1)
    {
        hx_drv_cis_init(g_xshutdown_pin, app_cust_config.we1_driver_cfg.mclk_div);
        app_get_sensor_id(&sensor_id, &rev_id);
        dbg_printf(DBG_LESS_INFO, "sensor_id=0x%04x,rev_id=0x%x\n", sensor_id, rev_id);
    }

    /*Sensor Configuration*/
    if (app_config_sensor_WE1_rx(sensor_init_required, SENSOR_STROBE_REQ) != 0)
    {
        dbg_printf(DBG_LESS_INFO, "app_config_sensor_WE1_rx fail\n");
        for (idx = 0; idx < MAX_RECONFIG_SENSOR_TIME; idx++)
        {
            if (app_config_sensor_WE1_rx(1, SENSOR_STROBE_REQ) != 0)
            {
                dbg_printf(DBG_LESS_INFO, "app_config_sensor_WE1_rx fail\n");
            }
            else
            {
                break;
            }
        }
        if (idx >= MAX_RECONFIG_SENSOR_TIME)
        {
            dbg_printf(DBG_LESS_INFO, "Sensor may crash\n");
            app_iot_facedetect_systemreset();
            return;
        }
    }
}


static int spi_write(uint32_t addr, uint32_t size, SPI_CMD_DATA_TYPE data_type)
{
#ifndef SPI_MASTER_SEND
	return hx_drv_spi_slv_protocol_write_simple_ex(addr, size, data_type);
#else
	return hx_drv_spi_mst_protocol_write_sp(addr, size, data_type);
#endif
}


void app_init_dpcbflag()
{
    g_xdma_abnormal = 0;
    g_rs_abnormal = 0;
    g_hog_abnormal = 0;
    g_rs_frameready = 0;
    g_hog_frameready = 0;
    g_md_detect = 0;
    g_cdm_fifoerror = 0;
    g_wdt1_timeout = 0;
    g_wdt2_timeout = 0;
    g_wdt3_timeout = 0;
    g_dp_event = 0;
    g_frame_ready = 0;
}


void app_cpu_sleep_at_capture()
{
#ifdef SUPPORT_CPU_SLEEP_AT_CAPTURE
    dbg_printf(DBG_APP_PRINT_LEVEL, "app_cpu_sleep_at_capture\n");
    PM_CFG_T aCfg;
    PM_CFG_PWR_MODE_E mode = PM_MODE_CPU_SLEEP;

    hx_lib_get_defcfg_bymode(&aCfg, mode);
    hx_lib_pm_mode_set(aCfg);
#endif
}



void synopsys_camera_start_capture (void)
{
    printf("\n\nversion 3.4\n\n");
    printf("start_capture() \n");

    evt_dp_clear_sensor_rtc();
    evt_dp_clear_all_dpevent();

    app_init_dpcbflag();

    printf("Set dma2\n");
    sensordplib_set_raw_wdma2(640, 480, NULL);
    
    printf("Set interrupt\n");
    hx_dplib_evthandler_register_cb(synopsys_camera_evt_handler, SENSORDPLIB_CB_FUNTYPE_DP);

    printf("Start_sensor_ctrl\n");
    sensordplib_set_sensorctrl_start();
}

/**
 * \brief	system reset
 *
 * \return	void.
 */
void app_iot_facedetect_systemreset()
{
    dbg_printf(DBG_APP_PRINT_LEVEL, "\n\n\n\n\nxxxx system reset xxxx\n");
    /*TODO*/
    hx_lib_pm_chip_rst(PMU_WE1_POWERPLAN_INTERNAL_LDO);
}



uint8_t cis_raw_algo_check(uint32_t img_addr)
{
    int read_status = 0;
    uint8_t next_jpeg_enc_frameno = 0;
    uint8_t cur_jpeg_enc_frameno = 0;
    uint32_t jpeg_enc_addr = 0;
    uint32_t jpeg_enc_filesize = 0;
    uint32_t de0_count;
    uint32_t convde_count;
    uint16_t af_framecnt;
    uint16_t be_framecnt;
    uint8_t wdma1_fin, wdma2_fin, wdma3_fin, rdma_fin;
    uint8_t ready_flag, nframe_end, xdmadone;

    /*Error handling*/
    if (g_xdma_abnormal != 0)
    {
        sensordplib_stop_capture();
        sensordplib_start_swreset();
        sensordplib_stop_swreset_WoSensorCtrl();
        dbg_printf(DBG_APP_PRINT_LEVEL, "abnormal re-setup path cur_frame=%d,acc=%d,event=%d\n", g_cur_hw5x5jpeg_frame, g_hw5x5jpeg_acc_frame, g_dp_event);
        g_xdma_abnormal = 0;
        g_hw5x5jpeg_err_retry_cnt++;
        //need re-setup configuration
        if (g_hw5x5jpeg_err_retry_cnt < MAX_HW5x5JPEG_ERR_RETRY_CNT)
        {
            // cis_raw_path(g_wdma2_baseaddr);
            synopsys_camera_start_capture(); 
        }
        else
        {
            dbg_printf(DBG_APP_PRINT_LEVEL, "hw5x5jpeg xdma fail overtime\n");
            app_iot_facedetect_systemreset();
        }
    }

    if ((g_wdt1_timeout == 1) || (g_wdt2_timeout == 1) || (g_wdt3_timeout == 1))
    {
        dbg_printf(DBG_APP_PRINT_LEVEL, "EDM WDT timeout event=%d\n", g_dp_event);
        hx_drv_edm_get_de_count(0, &de0_count);
        hx_drv_edm_get_conv_de_count(&convde_count);
        dbg_printf(DBG_APP_PRINT_LEVEL, "de0_count=%d, convde_count=%d\n", de0_count, convde_count);
        sensordplib_get_xdma_fin(&wdma1_fin, &wdma2_fin, &wdma3_fin, &rdma_fin);
        dbg_printf(DBG_APP_PRINT_LEVEL, "wdma1_fin=%d,wdma2_fin=%d,wdma3_fin=%d,rdma_fin=%d\n", wdma1_fin, wdma2_fin, wdma3_fin, rdma_fin);
        sensordplib_get_status(&ready_flag, &nframe_end, &xdmadone);
        dbg_printf(DBG_APP_PRINT_LEVEL, "ready_flag=%d,nframe_end=%d,xdmadone=%d\n", ready_flag, nframe_end, xdmadone);
        hx_drv_edm_get_frame_count(&af_framecnt, &be_framecnt);
        dbg_printf(DBG_APP_PRINT_LEVEL, "af_framecnt=%d,be_framecnt=%d\n", af_framecnt, be_framecnt);

        sensordplib_stop_capture();
        if (app_sensor_standby() != 0)
        {
            dbg_printf(DBG_APP_PRINT_LEVEL, "standby sensor fail 7\n");
        }
        sensordplib_start_swreset();
        sensordplib_stop_swreset_WoSensorCtrl();
        g_wdt1_timeout = 0;
        g_wdt2_timeout = 0;
        g_wdt3_timeout = 0;
        g_hw5x5jpeg_err_retry_cnt++;
        if (g_hw5x5jpeg_err_retry_cnt < MAX_HW5x5JPEG_ERR_RETRY_CNT)
        {
            app_sensor_xshutdown_toggle();
            if (app_config_sensor_WE1_rx(1, SENSOR_STROBE_REQ) != 0)
            {
                dbg_printf(DBG_APP_PRINT_LEVEL, "re-setup sensor fail\n");
            }
            synopsys_camera_start_capture(); 
        }
        else
        {
            dbg_printf(DBG_APP_PRINT_LEVEL, "hw5x5jpeg WDT fail overtime\n");
            app_iot_facedetect_systemreset();
        }
    }

    if (g_inp1bitparer_abnormal != 0)
    {
        dbg_printf(DBG_APP_PRINT_LEVEL, "g_inp1bitparer_err=%d\n", g_dp_event);
        g_hw5x5jpeg_err_retry_cnt++;
        g_inp1bitparer_abnormal = 0;
        if (g_hw5x5jpeg_err_retry_cnt < MAX_HW5x5JPEG_ERR_RETRY_CNT)
        {
            app_1bitparser_err_info();
            if (app_config_sensor_WE1_rx(1, SENSOR_STROBE_REQ) != 0)
            {
                dbg_printf(DBG_APP_PRINT_LEVEL, "re-setup sensor fail\n");
            }
            synopsys_camera_start_capture(); 
        }
        else
        {
            dbg_printf(DBG_APP_PRINT_LEVEL, "HW5x5JPEG 1bitparser Err retry overtime\n");
            app_iot_facedetect_systemreset();
        }
    }

    //Frame ready
    if (g_frame_ready == 1)
    {
        //tflitemicro_algo_run(img_addr, app_cust_config.we1_driver_cfg.jpeg_cfg.enc_width, app_cust_config.we1_driver_cfg.jpeg_cfg.enc_height, &algo_result);
        printf("Camera Get Data Success\n");

        uint8_t * img_ptr;
        uint32_t img_width = app_cust_config.we1_driver_cfg.jpeg_cfg.enc_width;
        uint32_t img_height = app_cust_config.we1_driver_cfg.jpeg_cfg.enc_height;
        img_ptr = (uint8_t *) img_addr;
        printf("Start to send \n");
        printf("Image width: %d\n", img_height);
        printf("Image height: %d\n", img_width);
        printf("Image address: 0x%08x\n", img_ptr);

        for(uint32_t heigth_cnt = 0; heigth_cnt < img_height; heigth_cnt ++)
        {
            for(uint32_t width_cnt = 0; width_cnt < img_width; width_cnt ++)
            {
                printf("%3d", *img_ptr);
                if(width_cnt != (img_width - 1))
                    printf(", ");
                else
                    printf("\n");

                img_ptr = img_ptr + 1;
            }
        }
        printf("End of send\n");

        /////////End//////////////////////
    } //frame ready
    return 0;
}


/*Sensor and RX related Start*/
int app_sensor_streaming()
{
	dbg_printf(DBG_LESS_INFO,"stream on\n");
	hx_drv_sensorctrl_set_MCLKCtrl(SENSORCTRL_MCLKCTRL_NONAOS);
	app_sensor_xsleep_ctrl(SENSORCTRL_XSLEEP_BY_CPU, 1);
	// stream on
	if(hx_drv_cis_setRegTable(app_cust_config.sensor_streamon_cfg.sensor_stream_cfg, app_cust_config.sensor_streamon_cfg.active_cfg_cnt)!= HX_CIS_NO_ERROR)
	{
		dbg_printf(DBG_LESS_INFO,"stream on by app fail\n");
		return -1;
	}
	return 0;
}


int app_get_sensor_id(uint16_t *sensor_id, uint8_t *rev_id)
{
	uint16_t addr;
	uint8_t val;

	//dbg_printf(DBG_LESS_INFO,"app_get_sensor_id\n");
	hx_drv_sensorctrl_set_MCLKCtrl(SENSORCTRL_MCLKCTRL_NONAOS);
	app_sensor_xsleep_ctrl(SENSORCTRL_XSLEEP_BY_CPU, 1);
	addr= 0x0000;
	if(hx_drv_cis_get_reg(addr, &val)!= HX_CIS_NO_ERROR)
	{
		dbg_printf(DBG_LESS_INFO,"get sensor id fail 1\n");
		return -1;
	}
	*sensor_id = val << 8;

	addr= 0x0001;
	if(hx_drv_cis_get_reg(addr, &val)!= HX_CIS_NO_ERROR)
	{
		dbg_printf(DBG_LESS_INFO,"get sensor id fail 2\n");
		return -1;
	}
	*sensor_id = *sensor_id | val;

	addr= 0x0002;
	if(hx_drv_cis_get_reg(addr, &val)!= HX_CIS_NO_ERROR)
	{
		dbg_printf(DBG_LESS_INFO,"get sensor id fail 3\n");
		return -1;
	}
	*rev_id = val;
	return 0;
}

/**
 * \brief	Sensor strobe on
 *
 * \return	int. (-1: failure, 0: success)
 */
int app_sensor_strobe_on()
{
	if (app_cust_config.sensor_table_cfg.sensor_id == SENSORDPLIB_SENSOR_HM0360_MODE1)
	{
		dbg_printf(DBG_LESS_INFO,"strobe on\n");
		hx_drv_sensorctrl_set_MCLKCtrl(SENSORCTRL_MCLKCTRL_NONAOS);
		app_sensor_xsleep_ctrl(SENSORCTRL_XSLEEP_BY_CPU, 1);
		// strobe on
		if (hx_drv_cis_setRegTable(app_cust_config.sensor_strobeon_cfg.sensor_strobe_on_cfg, app_cust_config.sensor_strobeon_cfg.active_cfg_cnt)!= HX_CIS_NO_ERROR)
		{
			dbg_printf(DBG_LESS_INFO,"strobe on by app fail\n");
			return -1;
		}
	}

	return 0;
}


/**
 * \brief	Non-AOS restreaming after 1 frame time
 *
 * \return	int. (-1: failure, 0: success)
 */
// Rowena TODO: maybe can delete
int app_nonAOS_restreaming()
{
	if(g_stream_type == SENSORDPLIB_STREAM_NONEAOS)
	{
	//FOR Non-AOS need streaming agaign, Toggle stream and standby should over 1 frame
#ifdef NONEAOS_TOGGLE_STREAM_STANDBY
		app_sensor_xsleep_ctrl(SENSORCTRL_XSLEEP_BY_CPU, 1);
		while(1)
		{
			g_tick_sensor_stream = _arc_aux_read(AUX_TIMER0_CNT);
			g_tick_sensor_toggle = g_tick_sensor_stream - g_tick_sensor_std;
			g_period = g_tick_sensor_toggle/BOARD_SYS_TIMER_MS_CONV;
			if(g_period > SENOSR_TOGGLE_STREAM_LIMITATION_MS)
			{
				dbg_printf(DBG_LESS_INFO,"Sensor toggle Period %d ms\r\n", g_period);
				break;
			}
		}
		if(app_sensor_streaming() !=0)
		{
			dbg_printf(DBG_LESS_INFO,"None-AOS re-streaming fail\r\n");
			return -1;
		}
		sensordplib_set_mclkctrl_xsleepctrl_bySCMode();
        #endif
	}
	return 0;
}


int app_sensor_standby()
{
	dbg_printf(DBG_LESS_INFO,"stream off by app\n");
	hx_drv_sensorctrl_set_MCLKCtrl(SENSORCTRL_MCLKCTRL_NONAOS);
	app_sensor_xsleep_ctrl(SENSORCTRL_XSLEEP_BY_CPU, 1);
	// stream off
	if(hx_drv_cis_setRegTable(app_cust_config.sensor_streamoff_cfg.sensor_off_cfg,app_cust_config.sensor_streamoff_cfg.active_cfg_cnt)!= HX_CIS_NO_ERROR)
	{
		dbg_printf(DBG_LESS_INFO,"stream off by app fail\n");
		return -1;
	}
	return 0;
}

int app_config_sensor_WE1_rx(uint8_t sensor_init_required, uint8_t sensor_strobe_req)
{
	if(sensor_init_required == 1)
	{
		app_config_sensor_all();
	}

	if(sensordplib_set_sensorctrl_inp(app_cust_config.sensor_table_cfg.sensor_id
			, app_cust_config.sensor_table_cfg.sensor_stream_type
			, app_cust_config.sensor_table_cfg.sensor_width
			, app_cust_config.sensor_table_cfg.sensor_height
			, app_cust_config.we1_driver_cfg.subsample) !=0)
	{
		dbg_printf(DBG_LESS_INFO,"sensordplib_set_sensorctrl_inp fail\n");
		return -1;
	}
	g_stream_type = app_cust_config.sensor_table_cfg.sensor_stream_type;

	if(sensor_init_required == 1)
	{
		if(sensor_strobe_req == 1)
		{
			app_sensor_strobe_on();
		}
		app_sensor_streaming();
	///	app_sensor_register_dump();
	}

	sensordplib_set_mclkctrl_xsleepctrl_bySCMode();

	hx_drv_pmu_set_ctrl(PMU_SEN_INIT, 0);
    return 0;
}

/**
 * \brief	configure Sensor all setting without streaming
 *
 * \return	int. (-1: failure, 0: success)
 */
int app_config_sensor_all()
{
	app_sensor_xsleep_ctrl(SENSORCTRL_XSLEEP_BY_CPU, 1);
	// not need set streaming mode keep in standby, it will config streaming mode in sensor_dplib
	if(hx_drv_cis_setRegTable(app_cust_config.sensor_table_cfg.sensor_cfg, app_cust_config.sensor_table_cfg.active_cfg_cnt)!= HX_CIS_NO_ERROR)
	{
		dbg_printf(DBG_LESS_INFO,"HM01B0 Config by app fail\n");
		return -1;
	}

	return 0;
}

/*
 * sensor ID: 	SENSORDPLIB_SENSOR_HM0360_MODE1
 *				SENSORDPLIB_SENSOR_HM0360_MODE2
 *				SENSORDPLIB_SENSOR_HM0360_MODE3
 *				SENSORDPLIB_SENSOR_HM0360_MODE5
 *				SENSORDPLIB_SENSOR_HM11B1_LSB
 *				SENSORDPLIB_SENSOR_HM11B1_MSB
 * */
void app_sensor_xsleep_ctrl(SENSORCTRL_XSLEEP_CTRL_E xsleep_ctrl, uint8_t val)
{
	if(app_cust_config.sensor_table_cfg.sensor_id == SENSORDPLIB_SENSOR_HM0360_MODE1)
	{
		if(xsleep_ctrl == SENSORCTRL_XSLEEP_BY_SC)
		{
			hx_drv_sensorctrl_set_xSleepCtrl(xsleep_ctrl);
		}else{
			hx_drv_sensorctrl_set_xSleepCtrl(xsleep_ctrl);
			hx_drv_sensorctrl_set_xSleep(val);
		}
	}
}

// Rowena: maybe can remove
void app_rx_set_wlcsp38_sharepin()
{	
	// Defined Config: WE1AppCfg_CHIP_Package_LQFP128
	if(app_cust_config.we1_driver_cfg.chip_package == WE1AppCfg_CHIP_Package_WLCSP38)
	{
       if((app_cust_config.sensor_table_cfg.sensor_id == SENSORDPLIB_SENSOR_HM11B1_LSB) ||
				(app_cust_config.sensor_table_cfg.sensor_id == SENSORDPLIB_SENSOR_HM11B1_MSB))
       {
    	   hx_drv_sensorctrl_set_wlcsp_sharepin(SENSORCTRL_WLCSP_SHAREPIN_HM11B1);
       }else if((app_cust_config.sensor_table_cfg.sensor_id == SENSORDPLIB_SENSOR_HM01B0_1BITIO_LSB) ||
    		   (app_cust_config.sensor_table_cfg.sensor_id == SENSORDPLIB_SENSOR_HM01B0_1BITIO_MSB))
       {
    	   hx_drv_sensorctrl_set_wlcsp_sharepin(SENSORCTRL_WLCSP_SHAREPIN_HM01B0);
       }
	}
}

void app_sensor_xshutdown_toggle()
{
	if(app_cust_config.sensor_table_cfg.sensor_id == SENSORDPLIB_SENSOR_HM0360_MODE1)
	{
		hx_drv_sensorctrl_set_xShutdown(g_xshutdown_pin, 0);
		board_delay_cycle(1* BOARD_SYS_TIMER_MS_CONV);
		hx_drv_sensorctrl_set_xShutdown(g_xshutdown_pin, 1);
		board_delay_cycle(50* BOARD_SYS_TIMER_US_CONV);
	}
}

void synopsys_camera_down_scaling (uint8_t * input_image, uint32_t input_width, uint32_t input_height, uint8_t * output_image, uint32_t output_width, uint32_t output_height)
{
    uint32_t input_ptr_index1 = 0;
    uint32_t input_ptr_index2 = 0;
    uint32_t input_ptr_index3 = 0;
    uint32_t output_ptr_index1 = 0;
    uint32_t output_ptr_index2 = 0;
    uint32_t width_step = 0;
    uint32_t height_step = 0;

    uint32_t width_i1 = 0;
    uint32_t height_i1 = 0;
    uint32_t width_i2 = 0;
    uint32_t height_i2 = 0;
    uint32_t pixel_buf = 0;
        
    width_step = input_width / output_width; 
    height_step = input_height / output_height; 


    printf("input height %3d, width = %3d\n", input_height, input_width);
    printf("output height %3d, width = %3d\n", output_height, output_width);
    printf("step height %3d, input index = %d\n", height_step, width_step);

    for(height_i1 = 0; height_i1 < output_height; height_i1 ++)
    {
        output_ptr_index1 = height_i1 * output_width;
        for(width_i1 = 0; width_i1 < output_width; width_i1 ++)
        {
            pixel_buf = 0;    
            input_ptr_index1 = (height_i1 * height_step * input_width) + (width_i1 * width_step);   
            for(height_i2 = 0; height_i2 < height_step; height_i2 ++)
            {
                input_ptr_index2 = height_i2 * input_width;
                for(width_i2 = 0; width_i2 < width_step; width_i2 ++)
                {
                    input_ptr_index3 = input_ptr_index1 + input_ptr_index2 + width_i2;
                    pixel_buf = pixel_buf + *(input_image + input_ptr_index3);
                }
            }
            output_ptr_index2 = output_ptr_index1 + width_i1;
            pixel_buf = pixel_buf / (width_step * height_step);
            if(pixel_buf > 255)
                pixel_buf = 255;
            *(output_image + output_ptr_index2) = pixel_buf;
        }
    }
}

