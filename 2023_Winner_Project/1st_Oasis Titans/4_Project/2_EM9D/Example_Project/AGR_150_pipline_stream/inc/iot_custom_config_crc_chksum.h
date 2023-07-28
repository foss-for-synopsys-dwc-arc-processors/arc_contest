/*
 * iot_custom_config_crc_chksum.h
 *
 *  Created on: 2019¦~8¤ë4¤é
 *      Author: 902447
 */

#ifndef SCENARIO_IOT_CONFIG_IOT_CUSTOM_CONFIG_CRC_CHKSUM_H_
#define SCENARIO_IOT_CONFIG_IOT_CUSTOM_CONFIG_CRC_CHKSUM_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "embARC.h"
#include "embARC_debug.h"
#include "board_config.h"

typedef union
{
	struct
	{
		uint8_t b0:1;
		uint8_t b1:1;
		uint8_t b2:1;
		uint8_t b3:1;
		uint8_t b4:1;
		uint8_t b5:1;
		uint8_t b6:1;
		uint8_t b7:1;
	};
	uint8_t byte;
} BYTE_T;

void HxGetCRC8(uint8_t *u8pData, uint32_t u32DataSize, uint8_t* pu8CRC8, uint16_t* pu16CheckSum);

#endif /* SCENARIO_APP_DEMO_SCENARIO_HUMAN_IOT_CONFIG_IOT_CUSTOM_CONFIG_CRC_CHKSUM_H_ */
