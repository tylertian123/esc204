#pragma once

#include "pico/stdlib.h"

/// @brief Namespace containing GPIO pin numbers of hardware components
namespace pinmap {
    constexpr uint z1_clk = 2,
                   z1_dir = 3,
                   z1_lower_lim = 28,
                   z1_upper_lim = 27;
    constexpr uint z2_clk = 4,
                   z2_dir = 5,
                   z2_lower_lim = 26,
                   z2_upper_lim = 22;
    constexpr uint x_clk = 6,
                   x_dir = 7,
                   x_lower_lim = 21;
    constexpr uint gripper = 8;
    constexpr uint lcd_d7 = 9,
                   lcd_d6 = 10,
                   lcd_d5 = 11,
                   lcd_d4 = 12,
                   lcd_en = 13,
                   lcd_rw = 14,
                   lcd_rs = 15;
    constexpr uint left = 18,
                   right = 17,
                   center = 16;
    constexpr uint slide1 = 20,
                   slide2 = 19;
                   
}

/// @brief Namespace containing configuration properties of hardware components
namespace hwconf {
    constexpr uint z1_freq = 1000;
    constexpr uint8_t z1_duty = 240;
    constexpr uint z2_freq = 1000;
    constexpr uint8_t z2_duty = 240;
    constexpr uint x_freq = 200;
    constexpr uint8_t x_duty = 70;

    // TODO: Change me!
    constexpr double x_mm_per_step = 0.5;

    constexpr uint16_t gripper_period = 20000,
                       gripper_min_pulse = 500,
                       gripper_max_pulse = 2500;
    // TODO: Change me!
    constexpr uint gripper_range = 180,
                   gripper_open = 0,
                   gripper_closed = 90;
}
