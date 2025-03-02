void mySetup()
{
  Serial.begin(9600);

  settingsMode = SettingsMode::SET_DRAINAGE_DELAY;
  potentiometerValue = analogRead(POTENTIOMETER_PIN) >> 2;

  // Start RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    // abort();
  }

  // Setup Seven Segment display
  byte numDigits = 4;
  byte digitPins[] = {8+2, 8+5, 8+6, 2}; // of ShiftRegister(s) | 8+x (2nd Register)
  byte segmentPins[] = {8+3, 8+7, 4, 6, 7, 8+4, 3,  5}; // of Shiftregister(s) | 8+x (2nd Register)
  bool resistorsOnSegments = true; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_ANODE; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected. Then, you only need to specify 7 segmentPins[]
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);

  // Setup control button
  button.attach(CONTROL_PIN, INPUT_PULLUP);
  button.interval(10);
  button.setPressedState(LOW);

  pinMode(CONTROL_LED, OUTPUT);

  water_level_sensor.setup();

  relay.setup();
}