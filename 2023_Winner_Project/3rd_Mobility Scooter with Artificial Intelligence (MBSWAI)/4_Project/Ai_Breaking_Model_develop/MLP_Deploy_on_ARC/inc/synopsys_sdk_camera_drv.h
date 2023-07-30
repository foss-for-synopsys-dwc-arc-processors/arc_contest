#ifndef SYNOPSYS_SDK_CAMERA_DRV_H_
#define SYNOPSYS_SDK_CAMERA_DRV_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "embARC.h"
#include "embARC_debug.h"
#include "board_config.h"
#include "arc_timer.h"

#include "spi_master_protocol.h"
#include "spi_slave_protocol.h"

#include "io_config_ext.h"
#include "sensor_dp_lib.h"
#include "apexextensions.h"
#include "powermode.h"

#include "event.h"
#include "event_handler.h"
#include "event_handler_evt.h"
#include "evt_datapath.h"

#include "hx_drv_CIS_common.h"
#include "hx_drv_spi_m.h"
#include "hx_drv_pmu.h"
#include "hx_drv_iomux.h"
#include "hx_drv_spi_s.h"


#include <app_macro_cfg.h>
#include <iot_custom_config_crc_chksum.h>
#include <iot_custom_config.h>
#include <iot_custom_defconfig.h>


#include "synopsys_sdk_camera_event_handler.h"

void synopsys_camera_init (void);
void synopsys_camera_start_capture (void);
void synopsys_camera_down_scaling (uint8_t * input_image, uint32_t input_width, uint32_t input_height, uint8_t * output_image, uint32_t output_width, uint32_t output_height);



uint8_t cis_raw_algo_check(uint32_t img_addr);


/**
 * \brief	configure WE-1 Sensor Control and INP related setting and Sensor Configuration
 *
 * \return	int. (-1: failure, 0: success)
 */
int app_config_sensor_WE1_rx(uint8_t sensor_init_required, uint8_t sensor_strobe_req);

/**
 * \brief	configure Sensor all setting without streaming
 *
 * \return	int. (-1: failure, 0: success)
 */
int app_config_sensor_all();

/**
 * \brief	configure Sensor to Standby in application
 *
 * \return	int. (-1: failure, 0: success)
 */
int app_sensor_standby();

/**
 * \brief	configure Sensor to Standby in application
 *
 * \return	int. (-1: failure, 0: success)
 */
int app_sensor_streaming();

/**
 * \brief	Sensor strobe on
 *
 * \return	int. (-1: failure, 0: success)
 */
int app_sensor_strobe_on();

/**
 * \brief	Sensor strobe off
 *
 * \return	int. (-1: failure, 0: success)
 */
int app_sensor_strobe_off();


void app_copy_sensor_default_cfg();

/*
 * sensor ID: 	SENSORDPLIB_SENSOR_HM0360_MODE1
 *				SENSORDPLIB_SENSOR_HM0360_MODE2
 *				SENSORDPLIB_SENSOR_HM0360_MODE3
 *				SENSORDPLIB_SENSOR_HM0360_MODE5
 *				SENSORDPLIB_SENSOR_HM11B1_LSB
 *				SENSORDPLIB_SENSOR_HM11B1_MSB
 * */
void app_sensor_xsleep_ctrl(SENSORCTRL_XSLEEP_CTRL_E xsleep_ctrl, uint8_t val);

void app_rx_set_wlcsp38_sharepin();

void app_sensor_xshutdown_toggle();

/**
 * \brief	configure WE-1 Sensor Control and INP related setting and Sensor MD Configuration
 *
 * \return	int. (-1: failure, 0: success)
 */
int app_config_sensor_md_WE1_rx(uint8_t sensor_init_required, uint8_t sensor_strobe_req);

/**
 * \brief	configure WE-1 Sensor Control and INP related setting and Sensor MD Configuration
 *
 * \return	int. (-1: failure, 0: success)
 */
int app_config_sensor_mddiffres_WE1_rx(uint8_t sensor_init_required, uint8_t sensor_strobe_req);


int app_get_sensor_id(uint16_t *sensor_id, uint8_t *rev_id);

#endif /* SYNOPSYS_SDK_CAMERA_DRV_H_ */
