#pragma once

#include <math.h>
#include <vector>
#include "pico/stdlib.h"

#include "slide.h"
#include "pinmap.h"
#include "stepper.h"
#include "servo.h"
#include "ui_hw.h"

// Note: Pin mappings and hardware config are defined in pinmap.h
namespace subsys {
    class ZMovement {
    public:
        /// @brief Position of the vertical movement mechanism.
        enum Position : uint8_t {
            /// @brief Unknown position. This occurs after reset and before calibration.
            UNKNOWN = 0,
            /// @brief Calibration position. This is all the way down.
            CALIBRATION,
            /// @brief Gripper raised.
            TOP,
            /// @brief Gripper lowered.
            BOTTOM
        };

    private:
        hw::A4988 left_stepper{pinmap::z1_clk, pinmap::z1_dir, hwconf::z1_freq, hwconf::z2_duty, hwconf::z1_inverted};
        hw::A4988 right_stepper{pinmap::z2_clk, pinmap::z2_dir, hwconf::z2_freq, hwconf::z2_duty, hwconf::z2_inverted};
        Position position = UNKNOWN;

    public:
        ZMovement();
        /// @brief Return whether the Z movement mechanism is currently moving.
        /// @return True if the Z movement mechanism is moving to a specified position
        bool busy() const;
        /// @brief Set the desired postion of the Z movement mechanism.
        /// @param pos The desired position; if UNKNOWN, the call has no effect
        void set_position(Position pos);
        /// @brief Get the position that the mechanism is in, or is moving towards.
        /// @return The current position (or if the mechanism is moving, the position it's moving towards)
        Position get_position() const;

        /// @brief Perform start-up calibration.
        void calibrate();
        /// @brief Perform start-up calibration. Does not return until done.
        void calibrate_blocking();
        /// @brief Pause steppers.
        void pause();
        /// @brief Resume steppers.
        void resume();
    };

    class XMovement {
    private:
        hw::A4988 stepper{pinmap::x_clk, pinmap::x_dir, hwconf::x_freq, hwconf::x_duty, hwconf::x_inverted};
        float position = NAN;
    
    public:
        XMovement();

        /// @brief Return whether the X movement mechanism is currently moving.
        /// @return True if the X movement mechanism is moving to a specified position
        bool busy() const;
        /// @brief Get the position that the mechanism is in, or is moving towards. NaN if uncalibrated.
        /// @return The current position in mm (or if the mechanism is moving, the position it's moving towards)
        float get_position() const;
        /// @brief Set the desired position of the X movement mechanism. Note this will be rounded to the nearest step.
        /// @param pos The desired position
        /// @return The actual position that was set (this may be different from the desired position due to step rounding)
        float set_position(float pos);

        /// @brief Perform start-up calibration.
        void calibrate();
        /// @brief Perform start-up calibration. Does not return until done.
        void calibrate_blocking();
        /// @brief Pause steppers.
        void pause();
        /// @brief Resume steppers.
        void resume();
    };

    class Gripper {
    private:
        hw::Servo servo{pinmap::gripper, hwconf::gripper_period, hwconf::gripper_min_pulse, hwconf::gripper_max_pulse, 180};
        uint32_t busy_until = 0;

    public:
        /// @brief Enum for the state of the gripper.
        enum State : bool {
            /// @brief Open state; evaluates to true.
            OPEN = true,
            /// @brief Closed state; evaluates to false.
            CLOSED = false
        };

        Gripper();

        /// @brief Set the gripper's state to open or closed.
        /// @param val Whether the gripper should be open or closed.
        void set(State val);

        /// @brief Return whether the gripper is currently opening or closing.
        /// Since position can't be read from the servo, this is determined based on the gripper opening/closing time specified in hwconf.
        /// @param t Current system time in ms; if zero, will be computed
        /// @return True if the gripper is currently opening or closing, false if gripper is idle
        bool busy(uint32_t t = 0) const;

        /// @brief Set the gripper's state to open or closed.
        /// @param val Whether the gripper should be open or closed.
        /// @return The gripper's new state
        State operator=(State val);
    };

    class Control {
    public:
        ZMovement z_axis;
        XMovement x_axis;
        Gripper gripper;
        
        // NOTE: a logical high on this pin DISABLES the steppers
        hw::GPIO stepper_enable{pinmap::stepper_enable, GPIO_OUT};
        hw::Button safety_sw{pinmap::safety};

        hw::Button slide_sw[Slide::SLOT_COUNTS[Slide::QUEUE]] = {
            hw::Button(pinmap::slide0),
            hw::Button(pinmap::slide1),
        };

        uint32_t last_calibrated = 0;

        enum class State : uint8_t {
            IDLE,           // System is free; no slide is being held
            MOVE_SLIDE,     // System is moving a slide between two slots
            CALIBRATION,    // System is performing auto-calibration
        };

        enum class Substate : uint8_t {
            X_MOVE_1 = 0,
            Z_DOWN_1,
            GRIPPER_CLOSE,
            Z_UP_1,
            X_MOVE_2,
            Z_DOWN_2,
            GRIPPER_OPEN,
            Z_UP_2,
            FINISHED,
        };

        enum class CalibrationSubstate : uint8_t {
            X,
            Z,
            RETURN,
            FINISHED,
        };

        State state = State::CALIBRATION;
        Substate substate = Substate::X_MOVE_1;
        Substate next_substate = Substate::X_MOVE_1;
        CalibrationSubstate calib_substate = CalibrationSubstate::X;
        CalibrationSubstate next_calib_substate = CalibrationSubstate::X;

        std::vector<Slide> slides;
        /// @brief Slide currently held by the gripper; nullptr if not holding anything.
        Slide *current_slide = nullptr;

        Control();

        /// @brief Run one cycle of the main slide stainer control algorithm.
        void run_once();

        void test_z_blocking(uint cycles);
        void test_x_blocking(uint cycles, double low_pos, double high_pos);
        void test_gripper_blocking(uint cycles);
    };

    extern Control control;
}
