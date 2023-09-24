#define FRACTION_BITS 8

void Mover::Init() {
  current_gain = 25;
  max_gain = 40;
  phase_fraction = 0;
  current_location = 0;
  delta_phase = 0;
  max_dph = 10;
  accel = 1;
  decel = 10;
}

void Mover::ResetLocation(int32_t new_loc) {
  noInterrupts();
  current_location = new_loc;
  target_location = new_loc;
  interrupts();
}

int32_t Mover::GetLocation() {
  int32_t ret;
  noInterrupts();
  ret = current_location;
  interrupts();
  return ret;
}

void Mover::UpdateLocation() {
  uint8_t st = DecodeEncoder(ReadEncoderPins());
  if (st) {
    if (st != encoder_state) {
//      debug_print = st + '0';
    }
    if (st==encoder_state) {
    } else if (st==NEXT_STATE(encoder_state)) {
      current_location ++;
    } else if (st==PREV_STATE(encoder_state)) {
      current_location --;
    } else {
      phase_errors ++;
    }
    encoder_state = st;
  }
}

void Mover::SetSpeed(int16_t s) {
  noInterrupts();
  max_dph = s;
  interrupts();
}

void Mover::SetTarget(int32_t t) {
  noInterrupts();
  target_location = t;
  interrupts();
}

int32_t Mover::GetTarget() {
  int32_t ret;
  noInterrupts();
  ret = target_location;
  interrupts();
  return ret;
}

inline int16_t Mover::CheckLimits(int16_t dp) {
  if (dp > max_dph)
    dp = max_dph;
  else if (dp < -max_dph)
    dp = -max_dph;
  if ((endstops & 1) and dp > 0)
    dp = 0;
  else if ((endstops & 2) and dp < 0)
    dp = 0;
   return dp;
}

#define MAX_FRACTION ((FULL_CIRCLE) << (FRACTION_BITS))

int16_t Mover::CalcMove() {
  int32_t dloc;
  int16_t dp = delta_phase;
  dloc = target_location - current_location;
  if (dloc > 0)
    if (dp >= 0)
      dp += accel;
    else
      dp += decel;
  else if (dloc < 0)
    if (dp <= 0)
      dp -= accel;
    else
      dp -= decel;
  else
    dp = 0;
  if (dp > max_dph)
    dp = max_dph;
  else if (dp < -max_dph)
    dp = -max_dph;
  if ((endstops & 1) and dp > 0)
    dp = 0;
  else if ((endstops & 2) and dp < 0)
    dp = 0;
  delta_phase = dp;
  phase_fraction += delta_phase;
  if (phase_fraction >= MAX_FRACTION)
    phase_fraction -= MAX_FRACTION;
  if (phase_fraction < 0)
    phase_fraction += MAX_FRACTION;
  if (delta_phase) {
    current_gain = 12 + (((max_dph >> ((FRACTION_BITS) - 5)) * 11) >> 3);
    if (current_gain > PWM_LIMIT)
      current_gain = PWM_LIMIT;
  } else
    current_gain = 0;
  return phase_fraction >> FRACTION_BITS;
}

void Mover::Tick() {
  endstops = ReadEndstops();
  UpdateLocation();
  if (auto_enabled) {
    int16_t ph = CalcMove();
    UpdateSinControl(ph, current_gain);
  } else {
    UpdateSinControl(0, 0);
  }
}

#define PF(field, name) Serial.print("[" name ":"); Serial.print(field); Serial.print("]");

void Mover::Print() {
  PF(current_gain, "CG");
  PF(phase_fraction, "PF");
  PF(delta_phase, "dph");
  PF(current_location, "CL");
  PF(target_location, "TL");
  Serial.println();
}

#undef PF

/*
int16_t Mover::CalcMove() {
  // return phase
  return phase_fraction >> FRACTION_BITS;
}
*/
