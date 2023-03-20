#pragma once

#include <limits.h>
#include "pico/stdlib.h"

namespace hw {
    class GPIO {
    private:
        uint pin = UINT_MAX;
    
    public:
        /// @brief Default constructor, leaves the pin uninitialized.
        /// reinit() MUST be called prior to using the pin.
        GPIO();
        /// @brief Constructor.
        /// @param pin GPIO pin number
        /// @param out Pin mode; set to true (or equivalently GPIO_OUT) for output pins, false or GPIO_IN for input pins
        GPIO(uint pin, bool out);
        /// @brief Constructor that also sets the internal pull-up/down.
        /// @param pin GPIO pin number.
        /// @param out Pin mode; set to true (or equivalently GPIO_OUT) for output pins, false or GPIO_IN for input pins
        /// @param pull_up If true, sets the interval pull-up
        /// @param pull_down If true, sets the internal pull-down
        GPIO(uint pin, bool out, bool pull_up, bool pull_down);

        /// @brief Reinitialize the GPIO pin in a new direction
        void reinit(bool out);
        /// @brief Reinitialize this object to use a different GPIO pin, in a new direction
        /// Note if this pin was previously initialized, the old pin is not reset!
        /// @param pin GPIO pin number
        /// @param out Pin mode
        void reinit(uint pin, bool out);
        /// @brief Attach the internal pull-up/down.
        /// @param up If true, attaches the internal pull-up
        /// @param down If true, attaches the internal pull-down
        void set_pull(bool up, bool down);

        /// @brief Set the output logic level on the pin.
        /// @param val The output to set
        /// @return The output value
        bool operator=(bool val);
        /// @brief Get the reading on the GPIO pin.
        operator bool() const;
    };
}
