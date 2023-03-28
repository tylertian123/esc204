#include "subsys.h"

#include <stdio.h>

#include "pinmap.h"
#include "stepper.h"
#include "util.h"

namespace subsys {
    ZMovement::ZMovement() {
        left_stepper.add_lower_lim_sw(pinmap::z1_lower_lim);
        right_stepper.add_lower_lim_sw(pinmap::z2_lower_lim);
    }

    bool ZMovement::busy() const {
        return !left_stepper.done() || !right_stepper.done();
    }

    ZMovement::Position ZMovement::get_position() const {
        return position;
    }

    void ZMovement::set_position(Position pos) {
        int steps = 0;
        switch (pos) {
        case CALIBRATION:
            steps = INT_MIN;
            break;
        case TOP:
            if (position == BOTTOM) {
                steps = hwconf::z_upper_pos_steps - hwconf::z_lower_pos_steps;
            }
            else if (position == CALIBRATION) {
                steps = hwconf::z_upper_pos_steps;
            }
            break;
        case BOTTOM:
            if (position == TOP) {
                steps = hwconf::z_lower_pos_steps - hwconf::z_upper_pos_steps;
            }
            else if (position == CALIBRATION) {
                steps = hwconf::z_lower_pos_steps;
            }
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

    XMovement::XMovement() {
        stepper.add_lower_lim_sw(pinmap::x_lower_lim);
    }

    bool XMovement::busy() const {
        return !stepper.done();
    }

    double XMovement::get_position() const {
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

    Gripper::Gripper() {
        set(OPEN);
    }

    void Gripper::set(Gripper::State state) {
        servo = state ? hwconf::gripper_open : hwconf::gripper_closed;
        busy_until = util::millis() + hwconf::gripper_change_duration;
    }

    Gripper::State Gripper::operator=(Gripper::State state) {
        set(state);
        return state;
    }

    bool Gripper::busy(uint32_t t) const {
        return t ? t > busy_until : util::millis() > busy_until;
    }

    void Control::run_once() {
        uint32_t time = util::millis();
        // Don't do anything if motors are currently moving
        if (z_axis.busy() || x_axis.busy() || gripper.busy(time)) {
            return;
        }

        // Main state machine logic
        switch (state) {
        case State::MOVE_SLIDE:
            switch (substate) {
            case Substate::X_MOVE_1:
            case Substate::X_MOVE_2:
                x_axis.set_position(current_slide->get_slot_position());
                break;
            case Substate::Z_DOWN_1:
            case Substate::Z_DOWN_2:
                z_axis.set_position(ZMovement::BOTTOM);
                break;
            case Substate::GRIPPER_OPEN:
                gripper = Gripper::OPEN;
                // Reset the slide timer once the slide is dropped off
                current_slide->reset_timer(time);
                break;
            case Substate::GRIPPER_CLOSE:
                gripper = Gripper::CLOSED;
                // Change current_slide, so that the new slot position is correct
                current_slide->move_to_next();
                break;
            case Substate::Z_UP_1:
            case Substate::Z_UP_2:
                z_axis.set_position(ZMovement::TOP);
                break;
            case Substate::FINISHED:
                state = State::IDLE;
                break;
            }
            if (substate != Substate::FINISHED) {
                // Move to next substate
                substate = static_cast<Substate>(static_cast<uint8_t>(substate) + 1);
            }
            break;
        case State::IDLE:
            // TODO
        case State::CALIBRATION:
            // TODO
        default:
            break;
        }
    }
}
