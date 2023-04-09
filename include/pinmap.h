#pragma once

#include "pico/stdlib.h"

/// @brief Namespace containing GPIO pin numbers of hardware components
namespace pinmap {
    constexpr uint z1_clk = 2,
                   z1_dir = 3,
                   z1_lower_lim = 28;
    constexpr uint z2_clk = 4,
                   z2_dir = 5,
                   z2_lower_lim = 26;
    constexpr uint x_clk = 6,
                   x_dir = 7,
                   x_upper_lim = 21;
    constexpr uint stepper_enable = 15;
    constexpr uint gripper = 8;
    constexpr uint lcd_d7 = 9,
                   lcd_d6 = 10,
                   lcd_d5 = 11,
                   lcd_d4 = 12,
                   lcd_en = 13,
                   lcd_rs = 14;
    constexpr uint left = 18,
                   right = 17,
                   center = 16;
    constexpr uint slide0 = 20,
                   slide1 = 19;
    constexpr uint safety = 27;
                   
}

/// @brief Namespace containing configuration properties of hardware components
namespace hwconf {
    /// @brief Frequency (Hz) of z1 stepper.
    constexpr uint z1_freq = 1000;
    /// @brief Duty cycle (out of 255) of z1 stepper.
    constexpr uint8_t z1_duty = 240;
    /// @brief Frequency (Hz) of z2 stepper.
    constexpr uint z2_freq = 1000;
    /// @brief Duty cycle (out of 255) of z2 stepper.
    constexpr uint8_t z2_duty = 240;
    /// @brief Frequency (Hz) of x stepper.
    constexpr uint x_freq = 200;
    /// @brief Duty cycle (out of 255) of x stepper.
    constexpr uint8_t x_duty = 70;
    /// @brief Amount of vertical travel (mm) per step in the z axis.
    constexpr float z_mm_per_step = 8.0 / 200.0;
    // Whether to invert the directions of the axes
    // NOTE: On the prototype the exposed metal side of the dupont connector should face outwards
    constexpr bool z1_inverted = false,
                   z2_inverted = false,
                   x_inverted = false;
    // TODO: Change me!
    /// @brief Distance (mm) from calibration of the lowered/raised z position.
    constexpr float z_lower_pos_mm = 20.0,
                     z_upper_pos_mm = 120.0;
    /// @brief Number of steps (from calibration position) to reach the lowered/raised z position.
    constexpr uint z_lower_pos_steps = static_cast<uint>(z_lower_pos_mm / z_mm_per_step),
                   z_upper_pos_steps = static_cast<uint>(z_upper_pos_mm / z_mm_per_step);


    // TODO: Change me!
    /// @brief Amount of horizontal travel (mm) per step in the x axis.
    constexpr float x_mm_per_step = 26.042 * 3.14159265 / 200;
    // TODO: Change me!
    /// @brief Horizontal position (mm) of the x carriage when it hits the calibration switch.
    constexpr float x_cal_pos = 180;

    /// @brief PWM period, min pulse, and max pulse of the gripper.
    constexpr uint16_t gripper_period = 20000,
                       gripper_min_pulse = 500,
                       gripper_max_pulse = 2500;
    /// @brief Pulse width (us) of the open and closed positions of the gripper servo.
    constexpr uint16_t gripper_open = 1850,
                       gripper_closed = 1750;
    // TODO: Change me!
    /// @brief The amount of time (ms) the gripper takes to open or close.
    constexpr uint32_t gripper_change_duration = 250;

    // TODO: Change me!
    /// @brief The minimum amount of time (ms) to wait before auto-calibrating again.
    constexpr uint32_t calib_period = 180000;
}
