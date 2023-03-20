#pragma once

#include "pico/stdlib.h"

#include "stepper.h"

// Note: Pin mappings and hardware config are defined in pinmap.h
namespace subsys {
    class ZMovement {
    public:
        /// @brief Position of the vertical movement mechanism.
        enum Position : uint8_t {
            /// @brief Unknown position. This occurs after reset and before calibration.
            UNKNOWN = 0,
            TOP,
            BOTTOM
        };

    private:
        hw::A4988 left_stepper;
        Position position = UNKNOWN;

    public:
        ZMovement();
        /// @brief Return whether the Z movement mechanism is currently moving.
        /// @return True if the Z movement mechanism is moving to a specified position
        bool busy();
        // Set the desired position; using UNKNOWN will do nothing
        /// @brief Set the desired postion of the Z movement mechanism.
        /// @param pos The desired position; if UNKNOWN, the call has no effect
        void set_position(Position pos);
        // Get the position that the mechanism is in, or is moving towards
        /// @brief Get the position that the mechanism is in, or is moving towards.
        /// @return The current position (or if the mechanism is moving, the position it's moving towards)
        Position get_position();
    };
}
