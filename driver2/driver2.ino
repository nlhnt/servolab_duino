#include "fastIO.h"
#include "global.h"
#include "threephase.h"
#include "move.h"
#include <EEPROM.h>

uint8_t pwm_fill = 0;
volatile bool auto_enabled = false, voltage_ok = false;
volatile int32_t tick_count = 0;
uint8_t max_pwm = 35;

#define pinMode fastPinMode
#define digitalWrite fastDigitalWrite
#define digitalRead fastDigitalRead

uint8_t DecodeEncoder(uint8_t pins) {
  static const uint8_t map[8] = {
    0, 4, 2, 3, 6, 5, 1, 0};
  if (pins >= 8) return 0;
  return map[pins];
}

Mover mover;

void PrintState() {
  static char buf[3];
  Serial.write("encoder ");
  Serial.print(DecodeEncoder(ReadEncoderPins()));
  Serial.write("  stops ");
  Serial.print(ReadEndstops());
  Serial.print("  pwm ");
  Serial.print(pwm_fill);
  Serial.print(" ");
  Serial.print(max_pwm);
  Serial.print("  location ");
  Serial.print(mover.GetLocation());
  Serial.print("  phase errors ");
  Serial.print(mover.phase_errors);
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_PH1_H, OUTPUT);
  pinMode(PIN_PH1_L, OUTPUT);
  pinMode(PIN_PH2_H, OUTPUT);
  pinMode(PIN_PH2_L, OUTPUT);
  pinMode(PIN_PH3_H, OUTPUT);
  pinMode(PIN_PH3_L, OUTPUT);
  pinMode(PIN_DBG1, OUTPUT);
  pinMode(PIN_DBG2, OUTPUT);
  pinMode(PIN_DBG3, OUTPUT);
  pinMode(PIN_STOP1, INPUT_PULLUP);
  pinMode(PIN_STOP2, INPUT_PULLUP);
  init_pwm();
  InitADC();
  mover.Init();
}

volatile char debug_print = 0;

ISR(TIMER1_OVF_vect){
  fastDigitalWrite(PIN_DBG1, 1);
  tick_count ++;
  mover.Tick();
  fastDigitalWrite(PIN_DBG1, 0);
}

void loop() {
  int32_t loc, buffer_num = 0;
  bool signed_num = false;
  uint8_t val;
  Serial.println("init");
  while (true) {
    if (debug_print) {
      char ch = debug_print;
      debug_print = 0;
      Serial.write(ch);
    }
//    if (adc_ph[0] > 10 || adc_ph[1] > 10 || adc_ph[2] > 10)
//      PrintVoltages();
    if (Serial.available()) {
      int ch = Serial.read();
      if (ch == 'L') {
        loc = mover.GetLocation();
        for (int8_t i=0; i<4; i++) {
          Serial.write(loc);
          loc >>= 8;
        }
        Serial.write(ReadEndstops());
        val = auto_enabled;
        val |= voltage_ok << 1;
        Serial.write(val);
      } else switch (ch) {
        case 'l':
          Serial.print("L");
          Serial.print(mover.GetLocation());
          Serial.print("E");
          Serial.print(ReadEndstops());
          val = auto_enabled;
          val |= voltage_ok << 1;
          Serial.print("S");
          Serial.println(val);
          break;
        case 'q':
          mover.SetTarget(mover.GetTarget() + 1);
          break;
        case 'a':
          mover.SetTarget(mover.GetTarget() - 1);
          break;
        case 'w':
          mover.SetTarget(mover.GetTarget() + 10);
          break;
        case 's':
          mover.SetTarget(mover.GetTarget() - 10);
          break;
        case 'e':
          mover.SetTarget(mover.GetTarget() + 300);
          break;
        case 'd':
          mover.SetTarget(mover.GetTarget() - 300);
          break;
        case ' ':
          PrintState();
          break;
        case 'Z':
          auto_enabled = true;
          break;
        case 'z':
          auto_enabled = false;
          break;
        case 'v':
          PrintVoltages();
          break;
        case 'V':
          InitADC();
          break;
        case '0' ... '9':
          buffer_num = buffer_num * 10 + ch - '0';
          break;
        case '-':
          signed_num = not signed_num;
          break;
        case 't':
          if (signed_num)
            buffer_num = -buffer_num;
          mover.SetTarget(buffer_num);
          buffer_num = 0;
          signed_num = false;
          break;
        case '*':
          buffer_num = 0;
          signed_num = false;
          break;
        case 'p':
          if (buffer_num > PWM_LIMIT)
            buffer_num = PWM_LIMIT;
          max_pwm = buffer_num;
          buffer_num = 0;
          signed_num = false;
          break;
        case 'S':
          mover.SetSpeed(buffer_num);
          buffer_num = 0;
          signed_num = false;
          break;
        case 'm':
          mover.Print();
          break;
        case 'i':
          Serial.print("ID");
          Serial.println(EEPROM.read(0));
          break;
        case 'I':
          EEPROM.update(0, buffer_num);
          buffer_num = 0;
          signed_num = 0;
          break;
        case 'b':
          fastDigitalWrite(PIN_LED, 0);
          break;
        case 'B':
          fastDigitalWrite(PIN_LED, 1);
          break;
        case 'U':
          mover.ResetLocation(buffer_num);
          buffer_num = 0;
          signed_num = 0;
          break;
      } // switch (ch)
    }
  }
}

/*
TODO
- wykrywanie braku 24V - niech trwa dluzej
- wykrywanie zwiechy keepalive
- stabilnosc enkodera/krancowej
*/
