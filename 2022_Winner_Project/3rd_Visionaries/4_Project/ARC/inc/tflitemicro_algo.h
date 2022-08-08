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

struct ans{
    int ercode_c;
    int ercode_i;
};

int tflitemicro_algo_init();
struct ans tflitemicro_algo_run_c(int8_t * ptr_image_c, int kImageSize_c, int8_t * ptr_image_i, int kImageSize_i);


#ifdef __cplusplus
}
#endif



#endif /* SCENARIO_APP_SAMPLE_CODE_PERIODICAL_WAKEUP_QUICKBOOT_TFLITEMICRO_ALGO_H_ */
