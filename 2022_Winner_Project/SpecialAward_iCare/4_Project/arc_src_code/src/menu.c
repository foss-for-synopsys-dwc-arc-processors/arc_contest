#include "menu.h"

menu initMenu(void (*eachLoop)()) {
  menu m = {0, {""}, optionSel, setOpt, eachLoop};
  return m;
};

void setOpt(struct __menu *m, char str[6][22], int optionNum) {
  memcpy(m->optionText, str, sizeof(char) * 6 * 22);
  m->optionNum = optionNum;
}

void renderOpt(struct __menu m) {
  char strBuf[22] = "";

  for (int i = 0; i < m.optionNum; i++) {
    OLED_SetCursor(i + 2, 0);
    sprintf(strBuf, "  %s", m.optionText[i]);
    OLED_DisplayString_Flush(strBuf);
  }
}

int optionSel(struct __menu m) {
  char strBuf[22] = "";
  int optionNum = m.optionNum;
  int optionPtr = 0;
  OLED_Clear();
  m.eachLoop();
  renderOpt(m);
  while (1) {
    m.eachLoop();
    OLED_SetCursor(optionPtr + 2, 0); // First 2 line is reserved for showtime()
    sprintf(strBuf, " ");
    OLED_DisplayString(strBuf);
    optionPtr += get_joystick_state();
    if (optionPtr > (optionNum - 1)) // -1 : cnt from 0
      optionPtr = 0;
    if (optionPtr < 0)
      optionPtr = optionNum - 1;
    OLED_SetCursor(optionPtr + 2, 0); // First 2 line is reserved for showtime()
    sprintf(strBuf, ">");
    OLED_DisplayString(strBuf);

    if (get_joystick_btn(JoyBtn)) {
      OLED_Clear();
      return optionPtr;
    } else
      board_delay_ms(200);
  }
}

void timeSel(int *hour, int *min) {
  char str_buf[20] = "";
  /*
  OLED_Clear();
  OLED_SetCursor(2, 0);
  sprintf(str_buf, "Time:");
  OLED_DisplayString(str_buf);
  */
  OLED_SetCursor(3, 0);
  sprintf(str_buf, "%02d:%02d", *hour, *min);
  OLED_DisplayString(str_buf);
SetHour:
  while (1) {
    *hour -= get_joystick_state();
    if (*hour < 0)
      *hour = 23;
    else if (*hour > 23)
      *hour = 0;
    OLED_SetCursor(3, 0);
    sprintf(str_buf, "  :%02d", *min);
    OLED_DisplayString(str_buf);
    board_delay_ms(100);

    OLED_SetCursor(3, 0);
    sprintf(str_buf, "%02d:%02d", *hour, *min);
    OLED_DisplayString(str_buf);
    board_delay_ms(100);
    if (get_joystick_btn(JoyBtn))
      break;
  }
  while (1) {
    *min -= get_joystick_state();
    if (*min < 0)
      *min = 59;
    else if (*min > 59)
      *min = 0;
    OLED_SetCursor(3, 0);
    sprintf(str_buf, "%02d:  ", *hour);
    OLED_DisplayString(str_buf);
    board_delay_ms(100);

    OLED_SetCursor(3, 0);
    sprintf(str_buf, "%02d:%02d", *hour, *min);
    OLED_DisplayString(str_buf);
    board_delay_ms(100);
    if (get_joystick_btn(JoyBtn))
      break;
    if (get_joystick_btn(JoyVRx)) // Go back
      goto SetHour;
  }
}

void setCalender(int *year, int *month, int *day) {
  char str_buf[22];
SetYear:
  while (1) {
    OLED_SetCursor(3, 0);
    sprintf(str_buf, "    /%02d/%02d", *month, *day);
    OLED_DisplayString(str_buf);
    board_delay_ms(100);

    OLED_SetCursor(3, 0);
    sprintf(str_buf, "%4d/%02d/%02d", *year, *month, *day);
    OLED_DisplayString(str_buf);
    board_delay_ms(100);

    *year -= get_joystick_state();
    if (*year < 1900 || *year > 9999)
      *year = 1900;
    if (get_joystick_btn(JoyBtn))
      break;
  }
SetMonth:
  while (1) {
    OLED_SetCursor(3, 0);
    sprintf(str_buf, "%4d/%02d/%02d", *year, *month, *day);
    OLED_DisplayString(str_buf);
    board_delay_ms(100);

    OLED_SetCursor(3, 0);
    sprintf(str_buf, "%4d/  /%02d", *year, *day);
    OLED_DisplayString(str_buf);
    board_delay_ms(100);

    *month -= get_joystick_state();
    if (*month < 1)
      *month = 12;
    else if (*month > 12) {
      *month = 1;
    }

    if (get_joystick_btn(JoyBtn))
      break;
    if (get_joystick_btn(JoyVRx)) // Go back
      goto SetYear;
  }
  bool isSmall =
      (*month == 4) || (*month == 6) || (*month == 9) || (*month == 11);
  bool isLeap =
      (((*year % 4 == 0) && (*year % 100 != 0)) || (*year % 400 == 0));
  while (1) {
    OLED_SetCursor(3, 0);
    sprintf(str_buf, "%4d/%02d/%02d", *year, *month, *day);
    OLED_DisplayString(str_buf);
    board_delay_ms(100);

    OLED_SetCursor(3, 0);
    sprintf(str_buf, "%4d/%02d/  ", *year, *month);
    OLED_DisplayString(str_buf);
    board_delay_ms(100);

    *day -= get_joystick_state();
    if (*month == 2) {
      if (isLeap) {
        if (*day < 1)
          *day = 29;
        else if (*day > 29)
          *day = 1;
      } else {
        if (*day < 1)
          *day = 28;
        else if (*day > 28)
          *day = 1;
      }
    } else if (isSmall) {
      if (*day < 1)
        *day = 30;
      else if (*day > 30)
        *day = 1;
    } else {
      if (*day < 1)
        *day = 31;
      else if (*day > 31)
        *day = 1;
    }

    if (get_joystick_btn(JoyBtn))
      break;
    if (get_joystick_btn(JoyVRx)) // Go back
      goto SetMonth;
  }
}

void sysTimeSetMenu(int *year, int *month, int *day, int *hour, int *min) {
  char str_buf[22];
  OLED_Clear();
  OLED_SetCursor(2, 0);
  sprintf(str_buf, "YYYY/MM/DD");
  OLED_DisplayString(str_buf);
  // Set Calender:
  setCalender(year, month, day);
  OLED_Clear();
  OLED_SetCursor(2, 0);
  sprintf(str_buf, "Time:");
  OLED_DisplayString(str_buf);
  timeSel(hour, min);
}