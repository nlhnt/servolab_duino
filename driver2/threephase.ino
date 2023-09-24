void ThreePhase_SetPhase(int8_t p) {
  if (p > 6 || p < 0) {
    Serial.println("invalid 3phase state");
    return;
  }
  static const uint8_t state_map[7][3] = {
    {2, 2, 2},
//*
    {2, 0, 1},
    {1, 0, 2},
    {1, 2, 0},
    {2, 1, 0},
    {0, 1, 2},
    {0, 2, 1},
//*/
/*
    {2, 1, 0},
    {0, 1, 2},
    {0, 2, 1},
    {2, 0, 1},
    {1, 0, 2},
    {1, 2, 0},
//*/
  };
}
