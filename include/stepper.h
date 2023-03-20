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

        /// @brief Constructor.
        /// @param clk Clock pin number
        /// @param dir Direction pin number
        /// @param freq Frequency of pulses in Hz
        /// @param duty Duty cycle (proportion of clock output that is high), out of 255
        A4988(uint clk, uint dir, uint32_t freq, uint8_t duty);
        
        static void pwm_wrap_cb_global(uint slice);
        void pwm_wrap_cb();

        // Add limit switches; these MUST be active-low

        /// @brief Attach a lower limit switch (reverse movement).
        /// @note The switch must be active low.
        /// @param pin Pin number of the switch
        void add_lower_lim_sw(uint pin);
        /// @brief Attach an upper limit switch (forward movement).
        /// @note The switch must be active low.
        /// @param pin Pin number of the switch
        void add_upper_lim_sw(uint pin);
        // Move num steps in the specified direction, asynchronously
        // If num == UINT_MAX, step indefinitely (until limit switch hits)

        /// @brief Move num steps in the specified direction, asynchronously
        /// @param num Number of steps; if UINT_MAX, step indefinitely (until limit switch hits)
        /// @param direction Direction of movement (true for forward, false for reverse)
        void step(uint num, bool direction);
        /// @brief Move num steps in the direction specified by sign of num, asynchronously
        /// @param num Number of steps; if negative, move in reverse; if INT_MAX or INT_MIN, step indefinitely (until limit switch hits)
        void step(int num);
        /// @brief Stop the movement completely
        void stop();
        /// @brief Return whether the stepper is done moving the specified number of steps
        /// @return True if the stepper is idle.
        bool done();
    };
}
