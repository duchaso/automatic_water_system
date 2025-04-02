#include "SevSegShift.h"
#include "Bounce2.h"
#include "elapsedMillis.h"
#include "RTClib.h"
#include "MagicPot.h"

#include "relay.hpp"
#include "water_level_sensor.hpp"
#include "conf.hpp"
#include "non_blocking_delay.hpp"



SettingsMode settingsMode = SettingsMode::SET_RTC_TRIGGER_TIME;
ButtonState buttonState = ButtonState::NOTHING;
ErrorCode errorCode = ErrorCode::NONE;

int step = 0;
int timeLeft = 10;
unsigned int drainageDelay = 30;
bool updateDrainageDelay = false;

DateTime rtcTriggerTime(2025, 2, 24, 2);
DateTime newTime(2025, 2, 24);



long potentiometerValue = map(drainageDelay, 0, 60, 0, 255);
DateTime now{};

Bounce2::Button button = Bounce2::Button();
SevSegShift sevseg(SHIFT_PIN_DS, SHIFT_PIN_SHCP, SHIFT_PIN_STCP); 
RTC_DS1307 rtc;
Relay relay(PUMP_PIN, THREE_WAY_VALVE_PIN, SOLENOID_PIN);
WaterLevelSensor water_level_sensor(WATER_LEVEL_L_PIN, WATER_LEVEL_H_PIN);
MagicPot potentiometer(POTENTIOMETER_PIN);
NonBlockingDelay timer;

/********************************** SETUP **********************************/
void setup() 
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
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros, disableDecPoint);

  // Setup control button
  button.attach(CONTROL_PIN, INPUT_PULLUP);
  button.interval(10);
  button.setPressedState(LOW);

  pinMode(CONTROL_LED, OUTPUT);

  water_level_sensor.setup();

  relay.setup();

  potentiometer.begin();
  potentiometer.onChange(onPotentiometerChange);

}

/********************************** LOOP **********************************/
void loop() 
{
  button.update();
  water_level_sensor.update();
  sevseg.refreshDisplay();
  potentiometer.read(POT_SENSITIVITY);
  if (rtc.isrunning())
    now = rtc.now();

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
    buttonState = getButtonState();
    processButtonState();
  }

  switch (step) {
    case 0:
    {
      if (!water_level_sensor.low_in_water() or 
        ( rtc.isrunning() and (rtcTriggerTime.unixtime() == now.unixtime()) ))
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
        timer.update_timer(drainageDelay * SEC_IN_MIN);
        updateDrainageDelay = false;
      }
      timeLeft = timer.wait_for(drainageDelay * SEC_IN_MIN);
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
        timeLeft = wait_for(SWITCHING_BACK_DELAY);
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
      timeLeft = wait_for(SOLENOID_DELAY);
      if (timeLeft <= 0) {
        relay.setSolenoid(Relay::OFF);
        step = 0;
      }
      break;
    }
  }

  display();
}