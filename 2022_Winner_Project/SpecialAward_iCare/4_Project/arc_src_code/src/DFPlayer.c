#include "DFPlayer.h"
void execute_cmd(uint8_t CMD, char Par1, char Par2) {
  // calcute checksum (2 bytes)
  uint16_t checksum =
      -(_Version_Byte + _Command_Length + CMD + _Acknowledge + Par1 + Par2);
  char highCheck = (char)(checksum >> 8);  // High byte of checksum
  char lowCheck = (char)(checksum & 0xFF); // Low byte of checksum
  // construct command;
  char Command_line[10] = {
      _Start_Byte, _Version_Byte, _Command_Length, CMD,      _Acknowledge,
      Par1,        Par2,          highCheck,       lowCheck, _End_Byte};

  // send cmd to module
  send_cmd(SC16IS750_PROTOCOL_SPI, Command_line, 10);
}

// The function playerBusy SHOULD be called ONLY in main.c
// Because in this lib we didn't use HX_GPIOSetup() to setup
// GPIO pin, which would cause undefined behaviou if we try
// to call the function
bool playerBusy(uint8_t busy_pin) {
  return GPIOGetPinState(SC16IS750_PROTOCOL_SPI, CH_A, busy_pin) ^ 1;
}

void pause() { execute_cmd(_PLAY_PAUSE, 0, 0); }

void play() {
  execute_cmd(_PLAY_START, 0, 1);
  board_delay_ms(100);
}

void playNext() {
  execute_cmd(_PLAY_NEXT, 0, 1);
  board_delay_ms(100);
}

void playPrev() {
  execute_cmd(_PLAY_PREV, 0, 1);
  board_delay_ms(100);
}

void playNum(uint16_t song_num) {
  char highNum = (char)(song_num >> 8);  // High byte of song_num
  char lowNum = (char)(song_num & 0xFF); // Low byte of song_num
  execute_cmd(_PLAY_NUM, highNum, lowNum);
  board_delay_ms(100);
}

void playFoldNum(char folder, char song_num) {
  execute_cmd(_PLAY_FOLD, folder, song_num);
  board_delay_ms(100);
}

void set_vol(uint8_t vol) { execute_cmd(_SET_VOL, 0, ((vol - 1) & 0x1f)); }

dfplayer Init_DFPlayer() {
  dfplayer dfp = {play,  playNum, playFoldNum, playNext,  playPrev,
                  pause, set_vol, execute_cmd, playerBusy};
  execute_cmd(_PLAY_Init, 0, 0);
  board_delay_ms(500);
  return dfp;
}

/* Should avoid dynamically allocated obj
dfplayer *Init_DFPlayer() {
  dfplayer *dfp = malloc(sizeof(dfplayer));
  dfp->play = play;
  dfp->playNum = playNum;
  dfp->next = playNext;
  dfp->prev = playPrev;
  dfp->pause = pause;
  dfp->set_vol = set_vol;
  dfp->sendCmd = execute_cmd;
  dfp->sendCmd(_PLAY_Init, 0, 0);
  board_delay_ms(500);
  return dfp;
}
*/