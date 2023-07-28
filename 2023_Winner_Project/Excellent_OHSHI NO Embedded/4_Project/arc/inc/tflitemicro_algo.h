/*
 * tflitemicro_algo.h
 *
 *  Created on: 2020�~5��27��
 *      Author: 902447
 */

#ifndef SCENARIO_APP_SAMPLE_CODE_PERIODICAL_WAKEUP_QUICKBOOT_TFLITEMICRO_ALGO_H_
#define SCENARIO_APP_SAMPLE_CODE_PERIODICAL_WAKEUP_QUICKBOOT_TFLITEMICRO_ALGO_H_

#ifdef __cplusplus
extern "C" {
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "embARC.h"
#include "embARC_debug.h"
#include "board_config.h"
#endif

typedef struct result_t result_t;

struct result_t {
    int8_t  BP0;
    int8_t  BP1;
};

int tflitemicro_algo_init();
result_t tflitemicro_algo_run(uint8_t * ptr_image);

#ifdef __cplusplus
}
#endif



#endif /* SCENARIO_APP_SAMPLE_CODE_PERIODICAL_WAKEUP_QUICKBOOT_TFLITEMICRO_ALGO_H_ */
