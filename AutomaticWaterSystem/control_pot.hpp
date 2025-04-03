#pragma once

#include <Arduino.h>

class ControlPot 
{
public:
    ControlPot(uint8_t pin, uint8_t sensitivity = 0) : m_pin{pin}, m_sensitiviy{sensitivity} {}

    void setup() 
    {
        m_value = analogRead(m_pin);
    }

    void update() 
    {
        int value = analogRead(m_pin);
        if (abs(value - m_value) > m_sensitiviy) {
            m_value = value;
        }
    }

    int value()
    {
        return m_value;
    }

private:
    uint8_t m_pin;
    int m_value;
    uint8_t m_sensitiviy;
};