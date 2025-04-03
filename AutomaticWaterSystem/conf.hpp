#pragma once
#include <Arduino.h>

/********************************** PINOUT ***************************************/
constexpr uint8_t SHIFT_PIN_DS          =2;   /* Data input PIN */
constexpr uint8_t SHIFT_PIN_STCP        =3;   /* Shift Register Storage PIN */
constexpr uint8_t SHIFT_PIN_SHCP        =4;   /* Shift Register Shift PIN */

constexpr uint8_t POTENTIOMETER_PIN     =0;   
constexpr uint8_t CONTROL_PIN           =9;   
constexpr uint8_t CONTROL_LED           =13;  

constexpr uint8_t WATER_LEVEL_L_PIN     =7;
constexpr uint8_t WATER_LEVEL_H_PIN     =8;

constexpr uint8_t SOLENOID_PIN          =12;
constexpr uint8_t THREE_WAY_VALVE_PIN   =11;
constexpr uint8_t PUMP_PIN              =10;

/********************************** GLOBAL ***************************************/
constexpr unsigned long SOLENOID_DELAY        = 1;
constexpr unsigned long DEFAULT_DRAINAGE_DELAY        = 30;
constexpr unsigned long SWITCHING_BACK_DELAY  = 1;
constexpr unsigned long PUMP_ABORT_TIME       = 15; // TODO 15
constexpr int POT_MAX_VAL                     = 1023;
constexpr uint8_t POT_SENSITIVITY             = 7;
constexpr unsigned long SEC_IN_MIN            = 1; // TODO 60

/********************************** ENUMS ****************************************/
enum class SettingsMode
{
  SET_DRAINAGE_DELAY,
  SET_RTC_TRIGGER_TIME,
  ADJUST_RTC_MODULE_H,
  ADJUST_RTC_MODULE_M,
  RUN_PUMP,
  OFF,
};

enum class ButtonState
{
  SHORT,
  LONG,
  NOTHING,
};

enum class ErrorCode
{
  H_IN_WATER_L_NOT_IN_WATER,
  PUMP_TIMEOUT,
  NONE,
};