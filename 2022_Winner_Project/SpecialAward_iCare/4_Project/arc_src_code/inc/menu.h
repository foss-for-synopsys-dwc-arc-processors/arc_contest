#include "synopsys_i2c_oled1306.h"
#include "hx_drv_timer.h"
#include "joystick.h"
#include <stdio.h>
#include <string.h>

struct __menu {
  int optionNum;
  char optionText[6][22];
  int (*sel)(struct __menu m);
  void (*setOpt)(struct __menu *m, char str[6][22], int optionNum);
  void (*eachLoop)(); // The func would be called through each loop
  void (*renderOpt)();
};
typedef struct __menu menu;

menu initMenu(void (*eachLoop)());
void setOpt(struct __menu *m, char str[6][22], int optionNum);
int optionSel(struct __menu m);
void renderOpt();
void sysTimeSetMenu(int *year, int *month, int *day, int *hr, int *min);
void timeSel(int *hour, int *min);
void setCalender(int *year, int *month, int *day);