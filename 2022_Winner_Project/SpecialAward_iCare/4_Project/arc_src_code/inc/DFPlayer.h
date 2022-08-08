#ifndef _DFPLAYER_H_
#define _DFPLAYER_H_

#define _Start_Byte 0x7E
#define _Version_Byte 0xFF
#define _Command_Length 0x06
#define _End_Byte 0xEF
#define _Acknowledge 0x00

#define _PLAY_NEXT 0x01
#define _PLAY_PREV 0x02
#define _PLAY_NUM 0x03
#define _SET_VOL 0x06
#define _PLAY_START 0x0D
#define _PLAY_PAUSE 0x0E
#define _PLAY_KEEP 0x11
#define _PLAY_Init 0x3F
#define _PLAY_FOLD 0x0F

#include "hx_drv_uart.h"
//#include "hx_drv_spi_s.h"
#include "board_config.h" //for board_delay_ms();

#include "DFPlayer.h"
#include <stdlib.h>

#include "SC16IS750_Bluepacket.h"

//#include "hx_drv_spi_s.h"
#include "board_config.h" //for board_delay_ms();

struct __dfplayer {
  void (*play)();
  void (*playNum)(uint16_t song_num);
  void (*playFoldNum)(char folder, char song_num);
  void (*playNext)();
  void (*playPrev)();
  void (*pause)();
  void (*set_vol)(uint8_t volume);
  void (*sendCmd)(uint8_t, char, char);
  bool (*isBusy)(uint8_t busy_pin);
};
typedef struct __dfplayer dfplayer;

void execute_cmd(uint8_t, char, char);
void pause();
void play();
void playNext();
void playPrev();
void set_vol(uint8_t vol);
void playNum(uint16_t song_num);
void playFoldNum(char folder, char song_num);
bool playerBusy(uint8_t busy_pin);
dfplayer Init_DFPlayer();
#endif