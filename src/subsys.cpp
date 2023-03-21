#include "subsys.h"

#include "pinmap.h"
#include "stepper.h"

namespace subsys {
    ZMovement::ZMovement()
        : left_stepper(pinmap::z1_clk, pinmap::z1_dir, hwconf::z1_freq, hwconf::z2_duty),
          right_stepper(pinmap::z2_clk, pinmap::z2_dir, hwconf::z2_freq, hwconf::z2_duty) {
        left_stepper.add_lower_lim_sw(pinmap::z1_lower_lim);
        right_stepper.add_lower_lim_sw(pinmap::z2_lower_lim);
    }

    bool ZMovement::busy() {
        return !left_stepper.done() || !right_stepper.done();
    }

    ZMovement::Position ZMovement::get_position() {
        return position;
    }

    void ZMovement::set_position(Position pos) {
        int steps = 0;
        switch (pos) {
        case CALIBRATION:
            steps = INT_MIN;
            break;
        case TOP:
            steps = hwconf::z_upper_pos_steps;
            break;
        case BOTTOM:
            steps = hwconf::z_lower_pos_steps;
            break;
        case UNKNOWN:
        default:
            break;
        }
        left_stepper.step(steps);
        right_stepper.step(steps);
        if (pos) {
            position = pos;
        }
    }

    void ZMovement::calibrate_blocking() {
        set_position(CALIBRATION);
        while (busy())
            tight_loop_contents();
    }

    XMovement::XMovement()
        : stepper(pinmap::x_clk, pinmap::x_dir, hwconf::x_freq, hwconf::x_duty) {
        stepper.add_lower_lim_sw(pinmap::x_lower_lim);
    }

    bool XMovement::busy() {
        return !stepper.done();
    }

    double XMovement::get_position() {
        return position;
    }

    double XMovement::set_position(double pos) {
        // Find how many steps is required and round to the nearest whole step
        long steps = round((pos - position) / hwconf::x_mm_per_step);
        position += steps * hwconf::x_mm_per_step;
        stepper.step(steps);
        return position;
    }

    void XMovement::calibrate_blocking() {
        stepper.step(INT_MIN);
        while (busy())
            tight_loop_contents();
        position = 0;
    }

    Gripper::Gripper()
        : servo(pinmap::gripper, hwconf::gripper_period, hwconf::gripper_min_pulse, hwconf::gripper_max_pulse, hwconf::gripper_range) {
        set(OPEN);
    }

    void Gripper::set(Gripper::State state) {
        servo = state ? hwconf::gripper_open : hwconf::gripper_closed;
    }

    Gripper::State Gripper::operator=(Gripper::State state) {
        set(state);
        return state;
    }
}
