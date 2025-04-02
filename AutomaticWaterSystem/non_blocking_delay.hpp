#pragma once
#include "elapsedMillis.h"

class NonBlockingDelay {
private:
    elapsedMillis timer;
    unsigned long remainingSeconds = 0;

public:
    // Start a new wait period or return remaining time
    unsigned long wait_for(unsigned long seconds) {
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

    void update_timer(unsigned long seconds) {
        if (remainingSeconds > 0) {
          remainingSeconds = seconds;
        }
    }
};