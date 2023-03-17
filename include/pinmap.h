#pragma once

#include "pico/stdlib.h"

// Namespace containing GPIO pin numbers of hardware components
namespace pinmap {
    constexpr uint z_left_clk = 15,
                   z_left_dir = 14,
                   z_lower_lim = 16,
                   z_upper_lim = 17;
}

// Namespace containing configuration properties of hardware components
namespace hwconf {
    constexpr uint z_left_freq = 1000;
    constexpr uint8_t z_left_duty = 240;
}
