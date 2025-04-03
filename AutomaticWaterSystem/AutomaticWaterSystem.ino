#if ENABLE_RTC
#include "RTClib.h"
#endif

#include "relay.hpp"
#include "water_level_sensor.hpp"
#include "conf.hpp"
#include "non_blocking_delay.hpp"
#include "control_pot.hpp"
#include "control_button.hpp"
#include "display.hpp"


SettingsMode settingsMode = SettingsMode::SET_RTC_TRIGGER_TIME;
ButtonState buttonState = ButtonState::NOTHING;
ErrorCode errorCode = ErrorCode::NONE;

int step = 0;
int timeLeft = 10;
unsigned int drainageDelay = DEFAULT_DRAINAGE_DELAY;
bool updateDrainageDelay = true;

#if ENABLE_RTC
DateTime rtcTriggerTime(2025, 2, 24, 2);
DateTime newTime(2025, 2, 24);
DateTime now{};
RTC_DS1307 rtc;
#endif

Display sevseg(SHIFT_PIN_DS, SHIFT_PIN_SHCP, SHIFT_PIN_STCP); 
Relay relay(PUMP_PIN, THREE_WAY_VALVE_PIN, SOLENOID_PIN);
WaterLevelSensor water_level_sensor(WATER_LEVEL_L_PIN, WATER_LEVEL_H_PIN);
ControlPot pot{POTENTIOMETER_PIN, 8};
ControlButton button{CONTROL_PIN};
NonBlockingDelay timer;

/********************************** SETUP **********************************/
void setup() 
{
  Serial.begin(9600);

  // Start RTC
  #if ENABLE_RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    // abort();
  }
  #endif

  // Setup Seven Segment display
  byte numDigits = 4;
  byte digitPins[] = {8+2, 8+5, 8+6, 2}; // of ShiftRegister(s) | 8+x (2nd Register)
  byte segmentPins[] = {8+3, 8+7, 4, 6, 7, 8+4, 3,  5}; // of Shiftregister(s) | 8+x (2nd Register)
  bool resistorsOnSegments = true; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_ANODE; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected. Then, you only need to specify 7 segmentPins[]
  sevseg.setup(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros, disableDecPoint);

  // Setup control button
  button.setup(INPUT_PULLUP, 10, LOW);  

  water_level_sensor.setup();

  relay.setup();

  pot.setup();
}

/********************************** LOOP **********************************/
void loop() 
{
  button.update();
  pot.update();
  water_level_sensor.update();
  sevseg.update();

  #if ENABLE_RTC
  if (rtc.isrunning())
    now = rtc.now();
  #endif

  healthCheck();

  #if 0
  Serial.print("Step: ");
  Serial.println(step);
  Serial.print("Mode: ");
  Serial.println((int)settingsMode);
  Serial.print("RTC: ");
  Serial.println(rtc.isrunning());
  Serial.println();

  if (! rtc.isrunning())
  {
    if (rtc.begin())
    {
      Serial.println("RTC STARTED");
    }
  }
  #endif

  if (step <= 0) {
    buttonState = button.value();
    processButtonState();
  }

  switch (step) {
    case 0:
    {
      if (!water_level_sensor.low_in_water() 
      #if ENABLE_RTC
        or ( rtc.isrunning() and (rtcTriggerTime.unixtime() == now.unixtime()) )
      #endif
      )
      {
        step = 1;
      }
      break;
    }
    case 1:
    {
      relay.set3WayValve(Relay::ThreeWayValveMode::DRAINAGE);
      relay.setPump(Relay::ON);
      step = 2;
      break;
    }
    case 2:
    {
      if (updateDrainageDelay == true) {
        timer.updateTimer(drainageDelay * SEC_IN_MIN);
        updateDrainageDelay = false;
      }
      timeLeft = timer.waitFor(drainageDelay * SEC_IN_MIN);
      if (timeLeft <= 0) {
        relay.set3WayValve(Relay::ThreeWayValveMode::TANK);
        step = 3;
      }
      break;
    }
    case 3:
    {
      if (water_level_sensor.high_in_water()) {
        relay.set3WayValve(Relay::ThreeWayValveMode::DRAINAGE);
        timeLeft = timer.waitFor(SWITCHING_BACK_DELAY);
        if (timeLeft <= 0) {
          relay.setPump(Relay::OFF);
          relay.setSolenoid(Relay::ON);
          step = 4;
        }
      }
      break;
    }
    case 4:
    {
      timeLeft = timer.waitFor(SOLENOID_DELAY);
      if (timeLeft <= 0) {
        relay.setSolenoid(Relay::OFF);
        step = 0;
      }
      break;
    }
  }
}