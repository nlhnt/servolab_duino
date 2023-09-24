class Mover {
  uint8_t encoder_state;
  int32_t current_location, target_location;
  int32_t phase_fraction;
  int16_t delta_phase, max_dph;
  int16_t accel, decel;
  uint8_t current_gain, max_gain;
  uint8_t endstops;
public:
  uint32_t phase_errors;
  void Init();
  void UpdateLocation();
  int32_t GetLocation();
  void SetTarget(int32_t);
  int32_t GetTarget();
  void SetSpeed(int16_t);
  int16_t CalcMove();
  void AutoMove();
  void Tick();
  void Print();
  int16_t CheckLimits(int16_t dp);
  void ResetLocation(int32_t new_loc);
};
