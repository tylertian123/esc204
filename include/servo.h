#pragma once

#include "pico/stdlib.h"

namespace util {
    class Servo {
    private:
        const uint pin;
        const uint slice, channel;
        uint16_t min_width, max_width;
        uint range;

    public:
        // pin is the output pin
        // period_us, min_width_us, max_width_us are the period, pulse width for min position, and pulse width for max position in microseconds
        // range is the angular range of the servo (in any integer unit)
        Servo(uint pin, uint16_t period_us, uint16_t min_width_us, uint16_t max_width_us, uint range);

        // Directly set the output pulse width in microseconds
        void set_output(uint16_t width_us);
        // Set the output angle
        void set_angle(uint angle);
        // Stop sending a signal to the servo
        // Note this means the servo will no longer hold its position!
        void stop();
    };
}
