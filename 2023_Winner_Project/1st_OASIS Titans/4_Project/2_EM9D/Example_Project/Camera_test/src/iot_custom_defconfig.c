/*
 * custom_config.c
 *
 *  Created on: 2019�~7��12��
 *      Author: 902447
 */
#include <app_cis_sensor_defcfg.h>
#include <iot_custom_defconfig.h>

#define CLASSIFIFCATION_FRAMERATE_CTRL    0
#define SENSOR_CDM_PERIODIC_TIMER_MS   500//1000
#define JPEG_DECODE_CHECK_FRAME    6
#define HW5X5_CHECK_MAX_FRAME      35

WE1AppCfg_CustTable_t  app_cust_config;
WE1AlgoCfg_CustTable_t  algo_cust_config;

#define WDMA3_BASE_ADDR   0x20124E70 //0x200FF670

extern void app_we1_default_cfg();

extern void app_we1_default_cfg()
{
// #ifdef IC_PACKAGE_WLCSP38
// 	app_cust_config.we1_driver_cfg.chip_package = WE1AppCfg_CHIP_Package_WLCSP38;
// #else
	app_cust_config.we1_driver_cfg.chip_package = WE1AppCfg_CHIP_Package_LQFP128;
//#endif
#if defined(CIS_HM0360_M_REVB_EXTLDO_MCLK24M) 
	app_cust_config.we1_driver_cfg.dp_clk_mux = WE1AppCfg_DP_CLK_Mux_XTAL;//WE1AppCfg_DP_CLK_Mux_RC36M;
	app_cust_config.we1_driver_cfg.mclk_clk_mux = WE1AppCfg_MCLK_CLK_Mux_XTAL;
	app_cust_config.we1_driver_cfg.mclk_div = SENSORCTRL_MCLK_DIV1;
#endif

// #ifdef CIS_XSHUT_SGPIO0
// 	app_cust_config.we1_driver_cfg.xshutdown_pin_sel = WE1AppCfg_GPIO_IOMUX_SGPIO0;
// #else
	app_cust_config.we1_driver_cfg.xshutdown_pin_sel = WE1AppCfg_GPIO_IOMUX_SGPIO1;
//#endif

	if(app_cust_config.app_table_cfg.app_type == WE1AppCfg_APPType_FACE_DETECT_ALLON)
		{
	// #ifdef EMZA_ALGO_FACE_V2 // Only Support QVGA
	// 		if(app_cust_config.sensor_table_cfg.sensor_color == WE1AppCfg_SensorColorType_MONO)
	// 		{
	// #ifdef SUPPORT_HW5X5_ONLY
	// 			app_cust_config.we1_driver_cfg.wdma1_startaddr = 0x2015D270;
	// 			app_cust_config.we1_driver_cfg.wdma2_startaddr = 0x2015D270;
	// 			app_cust_config.we1_driver_cfg.wdma3_startaddr = WDMA3_BASE_ADDR;//0x2015D270
	// #else
	// 			app_cust_config.we1_driver_cfg.wdma1_startaddr = 0x2015B470;
	// 			app_cust_config.we1_driver_cfg.wdma2_startaddr = 0x2015B470;
	// 			app_cust_config.we1_driver_cfg.wdma3_startaddr = WDMA3_BASE_ADDR;//0x2015D270
	// #endif
	// 			app_cust_config.we1_driver_cfg.jpegsize_autofill_startaddr = 0x2016FE70;
	// 		}else{
	// #ifdef SUPPORT_HW5X5_ONLY
	// 			app_cust_config.we1_driver_cfg.wdma1_startaddr = 0x20153C70;
	// 			app_cust_config.we1_driver_cfg.wdma2_startaddr = 0x20153C70;
	// 			app_cust_config.we1_driver_cfg.wdma3_startaddr = WDMA3_BASE_ADDR;//0x20153C70
	// 			app_cust_config.we1_driver_cfg.jpegsize_autofill_startaddr = 0x2016FE70;
	// #else
	// 			app_cust_config.we1_driver_cfg.wdma1_startaddr = 0x20150F70;
	// 			app_cust_config.we1_driver_cfg.wdma2_startaddr = 0x20150F70;
	// 			app_cust_config.we1_driver_cfg.wdma3_startaddr = WDMA3_BASE_ADDR;//0x20153C70
	// 			app_cust_config.we1_driver_cfg.jpegsize_autofill_startaddr = 0x2016FE70;
	// #endif
	// 		}
	// #else
			if(app_cust_config.sensor_table_cfg.sensor_color == WE1AppCfg_SensorColorType_MONO)
			{
	// #ifdef SUPPORT_HW5X5_ONLY
	// 			app_cust_config.we1_driver_cfg.wdma1_startaddr = 0x20124E70;
	// 			app_cust_config.we1_driver_cfg.wdma2_startaddr = 0x20124E70;
	// 			app_cust_config.we1_driver_cfg.wdma3_startaddr = WDMA3_BASE_ADDR;//0x20124E70
	// #else
				app_cust_config.we1_driver_cfg.wdma1_startaddr = 0x2011D670;
				app_cust_config.we1_driver_cfg.wdma2_startaddr = 0x2011D670;
				app_cust_config.we1_driver_cfg.wdma3_startaddr = WDMA3_BASE_ADDR;//0x20124E70
	//#endif
				app_cust_config.we1_driver_cfg.jpegsize_autofill_startaddr = 0x2016FE70;
			}else{
	// #ifdef SUPPORT_HW5X5_ONLY
	// 			app_cust_config.we1_driver_cfg.wdma1_startaddr = 0x200D7270;
	// 			app_cust_config.we1_driver_cfg.wdma2_startaddr = 0x200FF670;
	// 			app_cust_config.we1_driver_cfg.wdma3_startaddr = WDMA3_BASE_ADDR;//0x200FF670
	// #else
				app_cust_config.we1_driver_cfg.wdma1_startaddr = 0x200CBE70;
				app_cust_config.we1_driver_cfg.wdma2_startaddr = 0x200F4270;
				app_cust_config.we1_driver_cfg.wdma3_startaddr = WDMA3_BASE_ADDR;//0x200FF670
	//#endif
				app_cust_config.we1_driver_cfg.jpegsize_autofill_startaddr = 0x2016FE70;
			}
	//#endif
	// #ifdef SUPPORT_HW5X5_ONLY
	// 		app_cust_config.we1_driver_cfg.cyclic_buffer_cnt = 0;
	// #else
	#if defined(CIS_HM0360_M_REVB_EXTLDO_MCLK24M) 
			app_cust_config.we1_driver_cfg.cyclic_buffer_cnt = 1;
	#endif
	//#endif
	}
	
	

#if defined(CIS_HM0360_M_REVB_EXTLDO_MCLK24M) 
	app_cust_config.we1_driver_cfg.hw2x2_cfg.hw2x2_path = HW2x2_PATH_THROUGH;
	app_cust_config.we1_driver_cfg.hw2x2_cfg.hw_22_process_mode = HW2x2_MODE_BINNING;/**< HW2x2 Subsample Mode*/
	app_cust_config.we1_driver_cfg.hw2x2_cfg.hw_22_mono_round_mode = HW2x2_ROUNDMODE_FLOOR;/**< HW2x2 Rounding Mode*/
	app_cust_config.we1_driver_cfg.hw2x2_cfg.hw_22_crop_stx = 0;  /**< HW2x2 ROI Start X Position*/
	app_cust_config.we1_driver_cfg.hw2x2_cfg.hw_22_crop_sty = 0; /**< HW2x2 ROI Start Y Position*/
	app_cust_config.we1_driver_cfg.hw2x2_cfg.hw_22_in_width = 640;     /**< HW2x2 ROI Width*/
	app_cust_config.we1_driver_cfg.hw2x2_cfg.hw_22_in_height = 480;/**< HW2x2 ROI height*/
#endif
	app_cust_config.we1_driver_cfg.cdm_cfg.cdm_enable = CDM_ENABLE_ON;				/*!< CDM enable */
	app_cust_config.we1_driver_cfg.cdm_cfg.init_map_flag = CDM_INIMAP_FLAG_ON_ONEFRAME_ONLY;		/*!< CDM initial map flag */
	app_cust_config.we1_driver_cfg.cdm_cfg.cpu_activeflag = CDM_CPU_ACTFLAG_ACTIVE;		/*!< CDM cpu active flag */
	app_cust_config.we1_driver_cfg.cdm_cfg.meta_dump = CDM_ENABLE_MATA_DUMP_ON;		/*!< CDM meta data dump enable or disable */
	app_cust_config.we1_driver_cfg.cdm_cfg.ht_packing = CDM_ENABLE_HT_PACKING_ON;		/*!< CDM meta data dump packing or unpacking */
	app_cust_config.we1_driver_cfg.cdm_cfg.cdm_min_allow_dis = 3;				/*!< CDM algorithm minimum allowed distance between pixel and threshold setting*/
	app_cust_config.we1_driver_cfg.cdm_cfg.cdm_tolerance = 3;				/*!< CDM algorithm tolerance setting*/
	app_cust_config.we1_driver_cfg.cdm_cfg.cdm_reactance = 2;				/*!< CDM algorithm reactance setting*/
	app_cust_config.we1_driver_cfg.cdm_cfg.cdm_relaxation = 1;					/*!< CDM algorithm relaxation setting*/
	app_cust_config.we1_driver_cfg.cdm_cfg.cdm_eros_th = 3;				/*!< CDM algorithm erosion threshold setting*/
	app_cust_config.we1_driver_cfg.cdm_cfg.cdm_num_ht_th = 10;					/*!< CDM algorithm number of hot pixels threshold setting*/

	app_cust_config.we1_driver_cfg.cdm_cfg.cdm_num_ht_vect_th_x = 8;			/*!< CDM algorithm threshold for number of hot-pixels in each bin of the x-projection vector setting*/
	app_cust_config.we1_driver_cfg.cdm_cfg.cdm_num_ht_vect_th_y = 4;			/*!< CDM algorithm threshold for number of hot-pixels in each bin of the y-projection vector setting*/
	app_cust_config.we1_driver_cfg.cdm_cfg.cdm_num_cons_ht_bin_th_x = 1;		/*!< CDM algorithm threshold for number of consecutive hot bins of the x-projection vector setting*/
	app_cust_config.we1_driver_cfg.cdm_cfg.cdm_num_cons_ht_bin_th_y = 1;		/*!< CDM algorithm threshold for number of consecutive hot bins of the y-projection vector setting*/
#if defined(CIS_HM0360_M_REVB_EXTLDO_MCLK24M) 
	app_cust_config.we1_driver_cfg.cdm_cfg.cdm_crop_stx = 0;	/*!< CDM input ROI start x position*/
	app_cust_config.we1_driver_cfg.cdm_cfg.cdm_crop_sty = 0;	/*!< CDM input ROI start y position*/
	app_cust_config.we1_driver_cfg.cdm_cfg.cdm_in_width = 320;	/*!< CDM input ROI width*/
	app_cust_config.we1_driver_cfg.cdm_cfg.cdm_in_height = 240;	/*!< CDM input ROI height*/
#endif

	app_cust_config.we1_driver_cfg.hw5x5_cfg.fir_procmode = FIR_PROCMODE_LBP1;/**< HW5x5 FIR mode*/
	app_cust_config.we1_driver_cfg.hw5x5_cfg.firlpf_bndmode = FIRLPF_BNDODE_REPEAT;/**< HW5x5 FIR and LPF boundary extend mode*/
	app_cust_config.we1_driver_cfg.hw5x5_cfg.fir_lbp_th = 0x3;/**< HW5x5 FIR LBP Threshold*/
	app_cust_config.we1_driver_cfg.hw5x5_cfg.demos_pattern_mode = DEMOS_PATTENMODE_BGGR;/**< Bayer Sensor layout Selection for HW5x5*/
	app_cust_config.we1_driver_cfg.hw5x5_cfg.demoslpf_roundmode = DEMOSLPF_ROUNDMODE_FLOOR;/**< HW5x5 Demosaic and LPF Rounding mode*/

	app_cust_config.we1_driver_cfg.jpeg_cfg.jpeg_path = JPEG_PATH_ENCODER_EN;/**< JPEG Path Selection*/

#if defined(CIS_HM0360_M_REVB_EXTLDO_MCLK24M) 
	app_cust_config.we1_driver_cfg.hw5x5_cfg.hw5x5_path = HW5x5_PATH_BYPASS_5X5;/**< HW5x5 Path Selection*/
	app_cust_config.we1_driver_cfg.hw5x5_cfg.demos_color_mode = DEMOS_COLORMODE_YUV420;/**< HW5x5 Demosaic output Color Selection*/
	app_cust_config.we1_driver_cfg.hw5x5_cfg.demos_bndmode = DEMOS_BNDODE_INPUTEXTEND;/**< HW5x5 Demosaic Boundary process mode*/

	app_cust_config.we1_driver_cfg.hw5x5_cfg.hw55_crop_stx = 0;/**< HW5x5 ROI Start x-position*/
	app_cust_config.we1_driver_cfg.hw5x5_cfg.hw55_crop_sty = 0;/**< HW5x5 ROI Start y-position*/
	app_cust_config.we1_driver_cfg.hw5x5_cfg.hw55_in_width = 640;/**< HW5x5 ROI width*/
	app_cust_config.we1_driver_cfg.hw5x5_cfg.hw55_in_height = 480;/**< HW5x5 ROI height*/

	app_cust_config.we1_driver_cfg.jpeg_cfg.enc_width = 640; /**< JPEG encoding width(divide by 16)*/
	app_cust_config.we1_driver_cfg.jpeg_cfg.enc_height = 480; /**< JPEG encoding height(divide by 16)*/
	app_cust_config.we1_driver_cfg.jpeg_cfg.jpeg_enctype = JPEG_ENC_TYPE_YUV400;/**< JPEG Encoding Color format Selection*/
	// if(app_cust_config.app_table_cfg.app_type == WE1AppCfg_APPType_ALGODET_PERODICAL_WAKEUP_QUICKBOOT)
	// 	app_cust_config.we1_driver_cfg.jpeg_cfg.jpeg_encqtable = JPEG_ENC_QTABLE_4X;/**< JPEG Encoding quantization table Selection*/
	// else
		app_cust_config.we1_driver_cfg.jpeg_cfg.jpeg_encqtable = JPEG_ENC_QTABLE_10X;/**< JPEG Encoding quantization table Selection*/

	app_cust_config.we1_driver_cfg.jpeg_cfg.dec_roi_stx = 0; /**< JPEG Decoding ROI Start x-position(divide by 16)*/
	app_cust_config.we1_driver_cfg.jpeg_cfg.dec_roi_sty = 0; /**< JPEG Decoding ROI Start y-position(divide by 16)*/
	app_cust_config.we1_driver_cfg.jpeg_cfg.dec_width = 640; /**< JPEG Decoding ROI width(divide by 16)*/
	app_cust_config.we1_driver_cfg.jpeg_cfg.dec_height = 480;
#endif

	app_cust_config.we1_driver_cfg.analoge_pir_ch_sel = ADCC_CHANNEL0;
	app_cust_config.we1_driver_cfg.analoge_pir_th = 100;
	app_cust_config.we1_driver_cfg.light_sensor_adc_ch_sel = ADCC_CHANNEL1;
	app_cust_config.we1_driver_cfg.light_sensor_th = 100;
	if(app_cust_config.app_table_cfg.app_type == WE1AppCfg_APPType_FACE_DETECT_ALLON)
	{
		app_cust_config.we1_driver_cfg.pmu_type = PM_MODE_ALL_ON;
		app_cust_config.we1_driver_cfg.pmu_powerplan = PMU_WE1_POWERPLAN_INTERNAL_LDO;
		app_cust_config.we1_driver_cfg.bootspeed = PMU_BOOTROMSPEED_PLL_400M_50M;
		app_cust_config.we1_driver_cfg.pmu_skip_bootflow = 0;
		app_cust_config.we1_driver_cfg.support_bootwithcap = 0;
		app_cust_config.we1_driver_cfg.s_ext_int_mask = 0;
		app_cust_config.we1_driver_cfg.act_wakupCPU_pin_cnt = 0;
		app_cust_config.we1_driver_cfg.wakeupCPU_int_pin[0] = WE1AppCfg_GPIO_IOMUX_NONE;
	}

}

extern void app_iot_table_def_config()
{
#if	defined(APPTYPE_FACE_DETECT_ALLON)
	app_cust_config.app_table_cfg.app_type = WE1AppCfg_APPType_FACE_DETECT_ALLON;
	app_cust_config.app_table_cfg.pmu_sensor_rtc = 0;
	app_cust_config.app_table_cfg.pmu_sensor_wdg = 0;
// #if	CLASSIFIFCATION_FRAMERATE_CTRL
// 	app_cust_config.app_table_cfg.classification_rtc = 200;
// #else
	app_cust_config.app_table_cfg.classification_rtc = 0;
//#endif
	app_cust_config.app_table_cfg.adc_rtc = 0;/**< ADC RTC Interval */
	app_cust_config.app_table_cfg.adc_sample_period = 0;


	if(app_cust_config.app_table_cfg.app_type == WE1AppCfg_APPType_FACE_DETECT_ALLON)
	{
		app_cust_config.app_table_cfg.classification_detect_max_frame = HW5X5_CHECK_MAX_FRAME;/**< Live Capture for Full Detection Frame max numbers for no human:*/
		app_cust_config.app_table_cfg.detect_monitor_mode = WE1AppCfg_AlgoDetect_Monitor_BYDETECT;//WE1AppCfg_HumanDetect_Monitor_BYFRAMENO;//WE1AppCfg_HumanDetect_Monitor_BYHUMAN;/**< After Human detect, monitor stop method*/
		app_cust_config.app_table_cfg.nodetect_monitor_frame = 35;//0;/**< After Human detect, monitor stop frame*/
	}
	
	app_cust_config.app_table_cfg.cyclic_check_frame = JPEG_DECODE_CHECK_FRAME;
	app_cust_config.app_table_cfg.through_cv = WE1AppCfg_THROUGH_CV_YES;//WE1AppCfg_THROUGH_CV_YES; /**< Through CV or not*/

	//app_cust_config.app_table_cfg.motion_led_ind = WE1AppCfg_GPIO_IOMUX_PGPIO11;/**< Motion Led indication*/
	//app_cust_config.app_table_cfg.human_detect_ind = WE1AppCfg_GPIO_IOMUX_PGPIO5;/**< human detect Led indication*/
	app_cust_config.app_table_cfg.motion_led_ind = WE1AppCfg_GPIO_IOMUX_NONE;/**< Motion Led indication*/
	app_cust_config.app_table_cfg.human_detect_ind = WE1AppCfg_GPIO_IOMUX_NONE;/**< human detect Led indication*/
	app_cust_config.app_table_cfg.ir_led = WE1AppCfg_GPIO_IOMUX_NONE;/**< IR LED*/
	app_cust_config.app_table_cfg.light_support = WE1AppCfg_Ambient_Light_NO;/**< Ambient light Support*/
	app_cust_config.app_table_cfg.pdm_support = WE1AppCfg_PDM_Support_NO;
	app_cust_config.app_table_cfg.i2s_support = WE1AppCfg_I2S_Support_NO;

	app_copy_sensor_default_cfg();
	app_we1_default_cfg();

	app_cust_config.soc_comm_cfg.comm_type = WE1AppCfg_SOCCOMType_I2C_SLAVE;
#endif //Might not be correct location
}

extern void app_algo_table_def_config()
{
   algo_cust_config.algo_table_cfg.act_len = 200;
   memset(algo_cust_config.algo_table_cfg.para, 0x0, sizeof(algo_cust_config.algo_table_cfg.para));
   for(uint8_t idx = 0; idx < algo_cust_config.algo_table_cfg.act_len; idx++)
   {
	   algo_cust_config.algo_table_cfg.para[idx] = idx;
   }
}
