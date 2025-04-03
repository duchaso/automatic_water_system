#pragma once

#include <Arduino.h>

#include "elapsedMillis.h"
#include "SevSegShift.h"

class Display
{
public:
    Display(uint8_t pinDS, uint8_t pinSHCP, uint8_t pinSTCP) 
    : m_sevseg{SevSegShift(pinDS, pinSHCP, pinSTCP)} {}

    void setup(
        uint8_t hardwareConfig, 
        uint8_t numDigitsIn,
        uint8_t shiftRegisterMapDigits[],
        uint8_t shiftRegisterMapSegments[],
        bool resOnSegmentsIn, 
        bool updateWithDelaysIn, 
        bool leadingZerosIn,
        bool disableDecPoint
    ) 
    {
        m_sevseg.begin(hardwareConfig, 
            numDigitsIn,
            shiftRegisterMapDigits,
            shiftRegisterMapSegments,
            resOnSegmentsIn, 
            updateWithDelaysIn, 
            leadingZerosIn,
            disableDecPoint);
    }
    
    void update()
    {
        m_sevseg.refreshDisplay();
    }

    void setText(int32_t num, int8_t decPlaces = -1, bool blink = false)
    {
        updateDisplay([&]() { m_sevseg.setNumber(num, decPlaces); }, blink);
    }

    void setText(const char str[], bool blink = false)
    {
        updateDisplay([&]() { m_sevseg.setChars(str); }, blink);
    }

private: 
    template<typename Function>
    void updateDisplay(Function&& displayFunc, bool blink)
    {
        if (m_displayDelay > 10) {
            m_displayDelay = 0;
            if (blink) {
                if (m_blankDelay > 300) {
                    m_blankDelay = 0;
                    m_blank = !m_blank;
                    m_blank ? m_sevseg.blank() : displayFunc();
                }
            } else {
                displayFunc();
            }
        }
    }

    SevSegShift m_sevseg; 
    elapsedMillis m_displayDelay = 0;
    elapsedMillis m_blankDelay = 0;
    bool m_blank = false;
};