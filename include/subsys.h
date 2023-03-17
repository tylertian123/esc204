#pragma once

#include "pico/stdlib.h"

#include "stepper.h"

namespace subsys {
    class ZMovement {
    public:
        enum Position : uint8_t {
            UNKNOWN = 0,
            TOP,
            BOTTOM
        };

    private:
        hw::A4988 left_stepper;
        Position position = UNKNOWN;

    public:
        // Note: Pin mappings and hardware config are defined in pinmap.h
        ZMovement();

        // Return whether the Z movement mechanism is in the middle of moving
        bool busy();
        // Set the desired position; using UNKNOWN will do nothing
        void set_position(Position pos);
        // Get the position that the mechanism is in, or is moving towards
        Position get_position();
    };
}
