#pragma once

#include "pico/stdlib.h"

#include "gpio.h"

namespace util {
    class A4988 {
    private:
        const uint clk;
        const uint slice, channel;
        GPIO dir_pin;
        uint duty_cycle = 0;

        static A4988 *instances[8];
    
    public:
        uint steps_left = 0;
        // clk and dir are the pin numbers for clock and direction inputs
        // Frequency is in Hz (this cannot be too low or too high)
        // Duty is the duty cycle of the signal (out of 65535)
        A4988(uint clk, uint dir, uint32_t freq, uint8_t duty);
        
        static void pwm_wrap_cb_global(uint slice);
        void pwm_wrap_cb();

        // Move num steps in the specified direction, asynchronously
        void step(uint num, bool direction);
        // Stop the movement completely
        void stop();
        // Return whether the stepper is done moving the specified number of steps
        bool done();
    };
}
