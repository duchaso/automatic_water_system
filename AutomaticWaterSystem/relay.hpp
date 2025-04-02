#pragma once
#include "elapsedMillis.h"

class Relay 
{
public:
  static constexpr bool ON = true;
  static constexpr bool OFF = false;

  enum class ThreeWayValveMode 
  {
    DRAINAGE,
    TANK,
  };

  Relay(const int pump_pin, const int three_way_valve_pin, const int solenoid_pin)
  : pump_pin_(pump_pin),
    three_way_valve_pin_(three_way_valve_pin),
    solenoid_pin_(solenoid_pin)
    {}

  void setup()
  {
    setPump(OFF);
    setSolenoid(OFF);
    set3WayValve(ThreeWayValveMode::DRAINAGE);

    pinMode(solenoid_pin_, OUTPUT);
    pinMode(three_way_valve_pin_, OUTPUT);
    pinMode(pump_pin_, OUTPUT);
  }

  void setPump(bool state)
  {
    if (state == ON) pump_running_time_ = 0;
    pump_enabled_ = state;
    digitalWrite(pump_pin_, !state);
  }
  void setSolenoid(bool state)
  {
    digitalWrite(solenoid_pin_, !state);
  }
  void set3WayValve(ThreeWayValveMode mode)
  {
    switch (mode) {
      case ThreeWayValveMode::DRAINAGE:
        filling_tank_ = false;
        digitalWrite(three_way_valve_pin_, true);
        return;
      case ThreeWayValveMode::TANK:
      {
        pump_filling_tank_time_ = 0;
        filling_tank_ = true;
        digitalWrite(three_way_valve_pin_, false);
        return;
      }
      default:
        return;
    }
  }

  bool pumpEnabled() 
  {
    return pump_enabled_;
  }
  unsigned long pumpRunningTime()
  {
    if (pump_enabled_)
    {
      return pump_running_time_;
    } else {
      return 0;
    }
  }
  unsigned long pumpFillingTankTime()
  {
    return (pump_enabled_ and filling_tank_) ? (unsigned int)pump_filling_tank_time_ : 0;
  }
private:
  bool pump_enabled_ = false;
  bool filling_tank_ = false;
  elapsedMillis pump_running_time_ = 0;
  elapsedMillis pump_filling_tank_time_ = 0;

  int pump_pin_;
  int solenoid_pin_;
  int three_way_valve_pin_;
};
