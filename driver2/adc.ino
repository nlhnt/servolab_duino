volatile uint16_t adc_v24, adc_v12, adc_ph[3];
volatile int32_t adc_cycles = -1;
volatile int8_t adc_phase = 0;
uint8_t selected_phase = 0;

const uint8_t adc_clock = 0b111;

inline void ADCStartConversion(uint8_t ch) {
  ADMUX = 0b01000000 | ch;
  DIDR0 = 0b00111000;
  ADCSRA = 0b11001000 | adc_clock;
}

void ADCStartCycle(uint8_t ph) {
  selected_phase = ph;
  adc_phase = 0;
  ADCStartConversion(ph + 3);
}

inline void ADCStop() {
  ADCSRA = 0b10010000 | adc_clock;
}

void InitADC() {
  ADCStartCycle(0);
}

ISR(ADC_vect) {
  static uint8_t v24_bad_counter = 0;
  uint8_t ch;
//  fastDigitalWrite(PIN_DBG2, 1);
  switch (adc_phase) {
    case 0:
      adc_ph[selected_phase] = ADC;
      ch = 6;
      break;
    case 1:
      adc_v24 = ADC;
      if (adc_v24 > 600) {
        voltage_ok = true;
        v24_bad_counter = 0;
      } else {
        if (v24_bad_counter > 100)
          voltage_ok = false;
        else
          v24_bad_counter ++;
      }
      if (not voltage_ok)
        auto_enabled = false;
      ch = 7;
      break;
    case 2:
      adc_v12 = ADC;
      adc_phase = -1;
      adc_cycles ++;
      break;
  }
  if (adc_phase >= 0) {
    adc_phase ++;
    ADCStartConversion(ch);
  } else {
//    ADCStop();
    adc_phase = 0;
    ADCStartConversion(ch);
  }
//  fastDigitalWrite(PIN_DBG2, 0);
}

#define C(v)  v
//#define C(v)  ((int32_t(v))*6000*5/1023)

void PrintVoltages() {
  Serial.print(tick_count);
  Serial.print(" 24V ");
  Serial.print(C(adc_v24));
  Serial.print("  12V ");
  Serial.print(C(adc_v12));
  Serial.print("  PH ");
  Serial.print(C(adc_ph[0]));
  Serial.print(" ");
  Serial.print(C(adc_ph[1]));
  Serial.print(" ");
  Serial.print(C(adc_ph[2]));
  Serial.println();
}
#undef C

/*

s = v * t

 */
