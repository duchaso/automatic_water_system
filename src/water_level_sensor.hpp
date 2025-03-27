#pragma once

#include "Bounce2.h"

class WaterLevelSensor
{
public:
  WaterLevelSensor(const int pin_l, const int pin_h) 
  : pin_l_(pin_l),
    pin_h_(pin_h),
    low_(Bounce2::Button()),
    high_(Bounce2::Button())
  {}

  void setup() {
    low_.attach(pin_l_, INPUT_PULLUP);
    low_.interval(3000);
    low_.setPressedState(LOW);

    high_.attach(pin_h_, INPUT_PULLUP);
    high_.interval(2000);
    high_.setPressedState(LOW);
  }
  void update() {
    low_.update();
    high_.update();
  }
  bool low_in_water() {
    return low_.isPressed();
  }
  bool high_in_water() {
    return high_.isPressed();
  }
private:
  const int pin_l_;
  const int pin_h_;

  Bounce2::Button low_;
  Bounce2::Button high_;
};