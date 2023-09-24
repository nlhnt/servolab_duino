#ifndef GLOBAL_H__
#define GLOBAL_H__

#include "fastIO.h"

#define PIN_ENC1 A2
#define PIN_ENC2 A1
#define PIN_ENC3 A0
#define PIN_PH1_OUT A3
#define PIN_PH2_OUT A4
#define PIN_PH3_OUT A5
#define PIN_MSR_24 A6
#define PIN_MSR_12 A7

#define PIN_PH1_H 10  // OC1B
#define PIN_PH1_L 11  // OC2A
#define PIN_PH2_H 6   // OC0A
#define PIN_PH2_L 9   // OC1A
#define PIN_PH3_H 3   // OC2B
#define PIN_PH3_L 5   // OC0B
#define PIN_STOP1 8   // long
#define PIN_STOP2 7   // short

#define PIN_DBG1 2
#define PIN_DBG2 4
#define PIN_DBG3 12
#define PIN_LED 13

#define PWM_FULL 255
#define PWM_LIMIT 100
#define FULL_CIRCLE 768L

inline uint8_t ReadEncoderPins() {
  return (fastDigitalRead(PIN_ENC3) << 2) | (fastDigitalRead(PIN_ENC2) << 1) | fastDigitalRead(PIN_ENC1);
}

inline uint8_t ReadEndstops() {
  return (fastDigitalRead(PIN_STOP2) << 1) | fastDigitalRead(PIN_STOP1);
}

extern volatile int32_t tick_count;
extern volatile bool auto_enabled, voltage_ok;
extern uint8_t max_pwm;

extern volatile uint16_t adc_v24, adc_v12, adc_ph[3];
#define NEXT_STATE(state)  ((state) % 6 + 1)
#define PREV_STATE(state)  (((state) + 4) % 6 + 1)

extern uint8_t DecodeEncoder(uint8_t pins);
extern void init_pwm();

#endif
