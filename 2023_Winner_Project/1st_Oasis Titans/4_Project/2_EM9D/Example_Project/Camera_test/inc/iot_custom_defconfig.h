/*
 * iot_custom_defconfig.h
 *
 *  Created on: 2019¦~8¤ë5¤é
 *      Author: 902447
 */

#ifndef IOT_CUSTOM_DEFCONFIG_H_
#define IOT_CUSTOM_DEFCONFIG_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "embARC.h"
#include "embARC_debug.h"
#include "board_config.h"
#include "arc_timer.h"
#include "hx_drv_spi_s.h"
#include "spi_slave_protocol.h"
#include "hx_drv_iomux.h"
#include "sensor_dp_lib.h"
#include "hx_drv_CIS_common.h"
#include <algo_custom_config.h>
#include <app_macro_cfg.h>
#include <iot_custom_config.h>
#include "app_cis_sensor_defcfg.h"


void app_iot_table_def_config();
void app_algo_table_def_config();

#endif /* IOT_CUSTOM_DEFCONFIG_H_ */
