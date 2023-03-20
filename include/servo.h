#pragma once

#include "pico/stdlib.h"

namespace hw {
    class Servo {
    private:
        const uint pin;
        const uint slice, channel;
        uint16_t min_width, max_width;
        uint range;

    public:
        /// @brief Constructor.
        /// @param pin Output PWM pin
        /// @param period_us Total period in microseconds
        /// @param min_width_us Pulse width for minimum position in microseconds
        /// @param max_width_us Pulse width for maximum position in microseconds
        /// @param range Angular range in any unit
        Servo(uint pin, uint16_t period_us, uint16_t min_width_us, uint16_t max_width_us, uint range);

        /// @brief Directly set the output pulse width.
        /// @param width_us Output pulse width, in microseconds
        void set_output(uint16_t width_us);
        /// @brief Set the output angle (pulse width is interpolated).
        /// @param angle The output angle in any unit
        void set_angle(uint angle);
        /// @brief Stop sending a signal to the servo.
        /// @note The servo will no longer hold its position!
        void stop();

        /// @brief Set the output angle.
        /// @param angle The output angle in any unit
        /// @return The angle the servo was set to
        uint operator=(uint angle);
    };
}
