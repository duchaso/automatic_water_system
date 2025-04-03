#pragma once
#include "elapsedMillis.h"

class NonBlockingDelay {
public:
    // Start a new wait period or return remaining time
    unsigned long waitFor(unsigned long seconds) 
    {
        if (remainingSeconds == 0) {
          remainingSeconds = seconds;
          timer = 0;  // Reset timer
        }

        if (timer >= 1000) {
          timer -= 1000;
          --remainingSeconds;
        }

        return remainingSeconds;  // Return time left in seconds
    }

    void updateTimer(unsigned long seconds) 
    {
        if (remainingSeconds > 0) {
          remainingSeconds = seconds;
        }
    }

private:
    elapsedMillis timer;
    unsigned long remainingSeconds = 0;
};