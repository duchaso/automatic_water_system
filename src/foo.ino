#include "SevSegShift.h"
#include "Bounce2.h"
#include "elapsedMillis.h"
#include "RTClib.h"

#include "relay.hpp"
#include "water_level_sensor.hpp"

/********************************** PINOUT ***************************************/
constexpr int SHIFT_PIN_DS          =2;   /* Data input PIN */
constexpr int SHIFT_PIN_STCP        =3;   /* Shift Register Storage PIN */
constexpr int SHIFT_PIN_SHCP        =4;   /* Shift Register Shift PIN */

constexpr int POTENTIOMETER_PIN     =0;   
constexpr int CONTROL_PIN           =9;   
constexpr int CONTROL_LED           =13;  

constexpr int WATER_LEVEL_L_PIN     =7;
constexpr int WATER_LEVEL_H_PIN     =8;

constexpr int SOLENOID_PIN          =12;
constexpr int THREE_WAY_VALVE_PIN   =11;
constexpr int PUMP_PIN              =10;

/********************************** GLOBAL ***************************************/
constexpr unsigned long SOLENOID_DELAY        = 1;
constexpr unsigned long SWITCHING_BACK_DELAY  = 1;
constexpr unsigned long PUMP_ABORT_TIME       = 15; // TODO 15
constexpr int POT_MAX_VAL                     = 250;
constexpr unsigned long SEC_IN_MIN            = 60; // TODO 60

enum class SettingsMode
{
  SET_DRAINAGE_DELAY,
  SET_RTC_TRIGGER_TIME,
  ADJUST_RTC_MODULE_H,
  ADJUST_RTC_MODULE_M,
  RUN_PUMP,
  OFF,
} settingsMode;

enum class ButtonState
{
  SHORT,
  LONG,
  NOTHING,
} buttonState;

enum class ErrorCode
{
  H_IN_WATER_L_NOT_IN_WATER,
  PUMP_TIMEOUT,
} errorCode;

int step = 0;
int timeLeft = 10;
unsigned int drainageDelay = 30;
DateTime rtcTriggerTime(2025, 2, 24, 2);
DateTime newTime(2025, 2, 24);


long potentiometerValue = map(drainageDelay, 0, 60, 0, 255);
DateTime now{};

Bounce2::Button button = Bounce2::Button();
SevSegShift sevseg(SHIFT_PIN_DS, SHIFT_PIN_SHCP, SHIFT_PIN_STCP); 
RTC_DS1307 rtc;
Relay relay(PUMP_PIN, THREE_WAY_VALVE_PIN, SOLENOID_PIN);
WaterLevelSensor water_level_sensor(WATER_LEVEL_L_PIN, WATER_LEVEL_H_PIN);

/********************************** SETUP **********************************/
void setup() 
{
  mySetup();
}

/********************************** LOOP **********************************/
void loop() 
{
  button.update();
  water_level_sensor.update();
  sevseg.refreshDisplay();
  readPotentiometerValue();
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
      timeLeft = wait_for(drainageDelay);
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