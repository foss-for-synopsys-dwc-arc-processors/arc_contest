/*
 * sys_mem_config.h
 *
 *  Created on: 2020¦~8¤ë31¤é
 *      Author: 903935
 */

#ifndef INC_SYS_MEM_CONFIG_H_
#define INC_SYS_MEM_CONFIG_H_

#include "stdlib.h"

/**
* audio related buffer
*/

/**
 *  PDM buffer setting
 *
 *  ALANGO_STANDALONE should enable with RTOS.
 *  Otherwise, WEI can't receive audio real time
 */
#define AUD_BUF_BLOCK_SIZE 	        (4096)

#ifdef AIOT_HUMANDETECT_TV_CDM
    #define AUD_BUF_BLOCK_NUM       (16)
    #define AUD_BUF_ADDR            (0x20010800)
#else
#ifdef ALANGO_STANDALONE
    #define AUD_BUF_BLOCK_NUM       (10)//(32) 2sec, (64) 4sec //(20)   /* This constant defines the maximum number of Audio buffer */
    #define AUD_BUF_ADDR            malloc(AUD_BUF_BLOCK_SIZE*AUD_BUF_BLOCK_NUM)
#else
    #define AUD_BUF_BLOCK_NUM       (23)//(32) 2sec, (64) 4sec //(20)   /* This constant defines the maximum number of Audio buffer */
    #define AUD_BUF_ADDR            (0x20000000+36*1024)
#endif
#endif

/* Cyberon audio module */
#if defined(SUPPORT_TWO_LANGUAGE)
    #define AUDIO_APP_SRAM_ADDR       (0x200B2000)
#elif defined(ALANGO_STANDALONE)
	#ifdef AIOT_HUMANDETECT_AIRCONDITION
		#define AUDIO_APP_SRAM_ADDR       0x200F1000//(0x2011D000 - (164*1024))
	#else
    	#define AUDIO_APP_SRAM_ADDR       0x200F5000//(0x2011D000 - (164*1024))
	#endif
#else
	#ifdef TFLITE_MICRO_HIMAX_PERSON
		#define AUDIO_APP_SRAM_ADDR 		0x200F1000
	#else
		#ifdef USE_TONE_LIB
		#define AUDIO_APP_SRAM_ADDR       (0x2011D000 - (180*1024))
		#else
		#define AUDIO_APP_SRAM_ADDR       (0x80040000 - (170*1024)) //0x20000000//(0x200D3270)
		#endif
	#endif
#endif

/**
* Alango memory buffer
* and standalone memory address
*/
#ifdef ALANGO_STANDALONE
    #define ALANGO_PROFILE_ADDR(size)       0x20000000
#else
    #define ALANGO_PROFILE_ADDR(size)       malloc(size)
#endif

#ifdef AIOT_HUMANDETECT_AIRCONDITION
	#define ALANGO_STANDALONE_INIT_ADDR     0x200F1000//0x200F5000//0x200F4000
#else
	#define ALANGO_STANDALONE_INIT_ADDR     0x200F5000//0x200F5000//0x200F4000
#endif

#define AUD_BUF_MAX_ADDR	(0x20170000)

/**
* cyberon working memory (gSpotter.lpbyMemPool)  [global array]
* alango working memory  (reg[0].mem)            [memory allocation]
*/


/**
* video related buffer
*/
#define WDMA1_ADDR              0x2011D670
#define WDMA2_ADDR              0x2011D670
#define WDMA3_ADDR              0x20124E70
#define JPEG_SZ_AUTO_FILL_ADDR  0x2016FE70
/**
* algo - OKAO working memory(WMemoryBuff) [global array]
* algo - OKAO backup memory (BMemoryBuff) [global array]
*/

#endif /* INC_SYS_MEM_CONFIG_H_ */
