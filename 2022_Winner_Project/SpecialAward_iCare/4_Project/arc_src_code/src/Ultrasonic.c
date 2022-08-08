#include "UltraSonic.h"

void init_ultra() {
  GPIOSetPinMode(SC16IS750_PROTOCOL_SPI, CH_A, TrigPin, OUTPUT);
  GPIOSetPinMode(SC16IS750_PROTOCOL_SPI, CH_A, EchoPin, INPUT);
}

int detect_distance() {
  uint64_t startRec, endRec, duration;
  startRec = endRec = duration = 0;
  double cm = 0.;
  GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, TrigPin, LOW);
  board_delay_us(1);
  GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, TrigPin, HIGH);
  board_delay_us(11);
  GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, TrigPin, LOW);

  while (GPIOGetPinState(SC16IS750_PROTOCOL_SPI, CH_A, EchoPin) == LOW)
    startRec = board_get_cur_us();
  while (GPIOGetPinState(SC16IS750_PROTOCOL_SPI, CH_A, EchoPin) == HIGH)
    endRec = board_get_cur_us();
  duration = endRec - startRec;
  // printf("start:%llu end:%llu duration: %llu\n", startRec, endRec, duration);
  cm = ((double)duration) / 2.;
  cm = cm * 0.034;
  // printf("%d cm\n", (int)cm);
  return (int)cm;
}

bool detect_obj(int cm, int repeat) {
  int success = 0;
  for (int i = 0; i < repeat; ++i)
    if (detect_distance() <= cm) {
      success++;
      board_delay_ms(30);
    }
  return (success > (repeat / 2));
}
