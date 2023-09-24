#include "fastIO.h"
#include "global.h"

const uint8_t clock_div = 1;

void init_pwm1() {
  // OC1B PB2
  TIMSK1 = 0;
  TCCR1A = 0b11110001;
  TCCR1B = 0b00000000;
  TCNT1 = 1;
  OCR1B = 255;
  TIMSK1 = 1;
}

void init_pwm2() {
  // PH2_H D6 PD6 OC0A
  TIMSK0 = 0;
  TCCR0A = 0b11110001;
  TCCR0B = 0b00000000;
  TCNT0 = 1;
  OCR0A = 255;
}

void init_pwm3() {
  // PIN_PH3_H D3 PD3 OC2B
  TIMSK2 = 0;
  TCCR2A = 0b11110001;
  TCCR2B = 0b00000000;
  TCNT2 = 1;
  OCR2B = 255;
  ASSR = 0;
}

void init_pwm() {
  GTCCR = (1<<TSM)|(1<<PSRASY)|(1<<PSRSYNC);
  init_pwm1();
  init_pwm2();
  init_pwm3();
  TCCR1B |= clock_div;
  TCCR0B |= clock_div;
  TCCR2B |= clock_div;
  GTCCR = 0;
}
