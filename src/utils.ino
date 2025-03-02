// void blinkLed(const int pin, int times)
// {
//   static int ledState = LOW;
//   static int count = 0;
//   static elapsedMillis sinceLedStateChanged;
//   static elapsedMillis sinceBlinked;

//   if (count > 0 && sinceLedStateChanged > 300) {
//     sinceLedStateChanged = 0;
//     sinceBlinked = 0;

//     ledState = !ledState;
//     if (ledState == LOW) --count;
//     digitalWrite(pin, ledState);
//   }
  
//   if (count == 0 && sinceBlinked > 2000) {
//     count = times;
//   }
// }

void readPotentiometerValue()
{
  static long sum = 0;
  static int cnt = 0;
  static elapsedMillis controlReadTimeout;
  
  if (cnt == 20) {
    potentiometerValue = sum / cnt;
    cnt = 0;
    sum = 0;
  }

  if (controlReadTimeout > 5) {
    controlReadTimeout = 0;
    sum += (analogRead(POTENTIOMETER_PIN) >> 2);
    ++cnt;
  }
}

int wait_for(unsigned long minutes)
{
  static int remainingMinutes = 0;    // Track the remaining minutes
  static elapsedMillis elapsedTime = 0;        // Time elapsed in milliseconds

  // Initialize the countdown if it's the first call
  if (remainingMinutes <= 0)
  {
    remainingMinutes = minutes;
    elapsedTime = 0;  // Reset the timer
    return remainingMinutes;
  }

  // Check if 1 minute has passed
  if (elapsedTime >= 1 * SEC_IN_MIN * 1000)
  {
    elapsedTime -= 1 * SEC_IN_MIN * 1000;  // Reset the timer for the next second
    --remainingMinutes;   // Decrease remaining minutes by 1
  }

  return remainingMinutes;
}

void healthCheck()
{
  if (water_level_sensor.high_in_water() and !water_level_sensor.low_in_water())
  {
    errorCode = ErrorCode::H_IN_WATER_L_NOT_IN_WATER;
    raiseAbort();
    return;
  }

  if (relay.pumpFillingTankTime() > PUMP_ABORT_TIME * SEC_IN_MIN * 1000) 
  {
    errorCode = ErrorCode::PUMP_TIMEOUT;
    raiseAbort();
    return;
  }
}

void raiseAbort()
{
  relay.setPump(Relay::OFF);
  relay.set3WayValve(Relay::ThreeWayValveMode::DRAINAGE);
  relay.setSolenoid(Relay::OFF);

  settingsMode = SettingsMode::OFF;
  step = -1;
}