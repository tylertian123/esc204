#pragma once

#include <limits.h>
#include "pico/stdlib.h"

namespace util {
    class GPIO {
    private:
        uint pin = UINT_MAX;
    
    public:
        // Leave everything uninitialized
        // reinit() MUST be called prior to doing anything
        GPIO();
        // pin is the GPIO pin number
        // out is the pin mode; set to true (or equivalently GPIO_OUT) for output pins, false or GPIO_IN for input pins
        GPIO(uint pin, bool out);
        // pin is the GPIO pin number
        // out is the pin mode; set to true (or equivalently GPIO_OUT) for output pins, false or GPIO_IN for input pins
        // Set pull_up or pull_down to true to attach the internal pull up/down
        GPIO(uint pin, bool out, bool pull_up, bool pull_down);

        // Reinitialize the GPIO pin in a new direction
        void reinit(bool out);
        // Reinitialize this object to use a different GPIO pin, in a new direction
        // Note if this pin was previously initialized, the old pin is not reset!
        void reinit(uint pin, bool out);
        // Attach the interval pull up/down
        void set_pull(bool up, bool down);

        // Set the output on the pin
        bool operator=(bool val);
        // Get the input value on the pin
        operator bool() const;
    };
}
