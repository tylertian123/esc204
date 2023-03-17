#pragma once

#include "pico/stdlib.h"

#include "gpio.h"

namespace hw {
    class A4988 {
    private:
        const uint clk;
        const uint slice, channel;
        GPIO dir_pin;
        uint duty_cycle = 0;
        bool direction = 0;

        // Uninitialized by default
        GPIO lower_lim_sw, upper_lim_sw;
        bool has_lower_lim = false, has_upper_lim = false;

        static A4988 *instances[8];
    
    public:
        uint steps_left = 0;
        // clk and dir are the pin numbers for clock and direction inputs
        // Frequency is in Hz (this cannot be too low or too high)
        // Duty is the duty cycle of the signal (out of 65535)
        A4988(uint clk, uint dir, uint32_t freq, uint8_t duty);
        
        static void pwm_wrap_cb_global(uint slice);
        void pwm_wrap_cb();

        // Add limit switches; these MUST be active-low
        void add_lower_lim_sw(uint pin);
        void add_upper_lim_sw(uint pin);
        // Move num steps in the specified direction, asynchronously
        // If num == UINT_MAX, step indefinitely (until limit switch hits)
        void step(uint num, bool direction);
        // Move num steps, forward if num > 0, backward if num < 0
        // If num == INT_MAX or INT_MIN, step indefinitely (until limit switch hits)
        void step(int num);
        // Stop the movement completely
        void stop();
        // Return whether the stepper is done moving the specified number of steps
        bool done();
    };
}
