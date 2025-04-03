#pragma once

#include <Arduino.h>

#include "Bounce2.h"
#include "conf.hpp"

class ControlButton 
{
public:
    ControlButton(uint8_t pin) : m_pin{pin}, m_button{Bounce2::Button()} {}

    void setup(int mode, uint16_t debounceInterval = 0, bool pressed_state = LOW) 
    {
        m_button.attach(m_pin, mode);
        m_button.interval(debounceInterval);
        m_button.setPressedState(pressed_state);
    }

    void update() 
    {
        m_button.update();
        if (m_button.released()) {
            if (m_button.previousDuration() > 3000) {
            // long press
            m_value = ButtonState::LONG;
            } else {
            // short press
            m_value = ButtonState::SHORT;
            }
        } else {
            m_value = ButtonState::NOTHING;
        }
    }

    ButtonState value()
    {
        return m_value;
    }

private:
    uint8_t m_pin;
    ButtonState m_value;
    Bounce2::Button m_button;
};