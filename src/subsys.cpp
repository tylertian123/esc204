#include "subsys.h"

#include <stdio.h>
#include <limits.h>

#include "pinmap.h"
#include "stepper.h"
#include "util.h"
#include "slide.h"

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

    void ZMovement::calibrate() {
        set_position(CALIBRATION);
    }

    void ZMovement::calibrate_blocking() {
        calibrate();
        while (busy())
            tight_loop_contents();
    }

    void ZMovement::pause() {
        left_stepper.pause();
        right_stepper.pause();
    }

    void ZMovement::resume() {
        left_stepper.resume();
        right_stepper.resume();
    }

    XMovement::XMovement() {
        stepper.add_upper_lim_sw(pinmap::x_upper_lim);
    }

    bool XMovement::busy() const {
        return !stepper.done();
    }

    float XMovement::get_position() const {
        return position;
    }

    float XMovement::set_position(float pos) {
        // Find how many steps is required and round to the nearest whole step
        auto steps = static_cast<long>(round((pos - position) / hwconf::x_mm_per_step));
        position += steps * hwconf::x_mm_per_step;
        stepper.step(steps);
        return position;
    }

    void XMovement::calibrate() {
        stepper.step(INT_MAX);
        position = hwconf::x_cal_pos;
    }

    void XMovement::calibrate_blocking() {
        calibrate();
        while (busy())
            tight_loop_contents();
    }

    void XMovement::pause() {
        stepper.pause();
    }

    void XMovement::resume() {
        stepper.resume();
    }

    Gripper::Gripper() {
        set(OPEN);
    }

    void Gripper::set(Gripper::State state) {
        servo.set_output(state == OPEN ? hwconf::gripper_open : hwconf::gripper_closed);
        busy_until = util::millis() + hwconf::gripper_change_duration;
    }

    Gripper::State Gripper::operator=(Gripper::State state) {
        set(state);
        return state;
    }

    bool Gripper::busy(uint32_t t) const {
        return t ? t < busy_until : util::millis() < busy_until;
    }

    Control::Control() {
        // Set changed flag to true to do a check on startup
        safety_sw.changed = true;
    }

    void Control::run_once() {
        uint32_t time = util::millis();

        // First poll all buttons
        safety_sw.poll();
        for (uint i = 0; i < Slide::SLOT_COUNTS[Slide::QUEUE]; i ++) {
            slide_sw[i].poll(time);
        }

        // Handle safety switch
        if (safety_sw.changed) {
            safety_sw.changed = false;
            // Pause everything if safety switch is not down
            if (!safety_sw.down) {
                z_axis.pause();
                x_axis.pause();
                stepper_enable = true;
                return;
            }
            else {
                z_axis.resume();
                x_axis.resume();
                stepper_enable = false;
            }
        }

        // Handle system inputs
        for (uint i = 0; i < Slide::SLOT_COUNTS[Slide::QUEUE]; i ++) {
            if (slide_sw[i].down) {
                // Create the slide if not created already, after the switch has been held for a while
                if (slide_sw[i].held && state == State::IDLE && !Slide::slot_occupation[Slide::QUEUE][i]) {
                    slides.emplace_back(i, time, time);
                }
            }
            else {
                slide_sw[i].held = false;
                // If the switch is released but the slot is still occupied, remove the slide
                // Normally, the slide will have been moved in software before it is physically moved
                if (Slide::slot_occupation[Slide::QUEUE][i]) {
                    for (auto it = slides.begin(); it != slides.end(); ++it) {
                        // Find the slide in this slot and erase
                        if (it->stage == Slide::QUEUE && it->slot == i) {
                            // Clear current_slide pointer if needed
                            if (current_slide == &*it) {
                                current_slide = nullptr;
                            }
                            slides.erase(it);
                            Slide::slot_occupation[Slide::QUEUE][i] = nullptr;
                            break;
                        }
                    }
                }
            }
        }

        // Don't do anything if motors are currently moving
        if (z_axis.busy() || x_axis.busy() || gripper.busy(time)) {
            return;
        }

        // Main state machine logic
        switch (state) {
        // Move slide state
        // Entered when the gripper is opened, in the up position
        // Exits with the gripper opened, in the down position
        // Will move to the position of current_slide, pick it up, and move it to its next position
        case State::MOVE_SLIDE:
            substate = next_substate;
            // Note, if a queued slide is being moved, there is a chance that current_slide = nullptr
            // If the slide is removed
            switch (substate) {
            case Substate::X_MOVE_1:
            case Substate::X_MOVE_2:
                // Directly exit if slide was removed
                if (!current_slide) {
                    substate = Substate::FINISHED;
                    break;
                }
                x_axis.set_position(current_slide->get_slot_position());
                break;
            case Substate::Z_DOWN_1:
            case Substate::Z_DOWN_2:
                if (!current_slide) {
                    substate = Substate::FINISHED;
                    break;
                }
                z_axis.set_position(ZMovement::BOTTOM);
                break;
            case Substate::GRIPPER_OPEN:
                gripper = Gripper::OPEN;
                // If slide removed, first open the gripper, then move to the next state
                // The next state will bring the Z axis up
                if (!current_slide) {
                    break;
                }
                // Reset the slide timer once the slide is dropped off
                current_slide->reset_timer();
                current_slide->in_transit = false;
                break;
            case Substate::GRIPPER_CLOSE:
                // If slide remved, don't close the gripper, and move to the next state to bring the Z axis up
                if (!current_slide) {
                    break;
                }
                gripper = Gripper::CLOSED;
                // Change current_slide, so that the new slot position is correct
                current_slide->move_to_next();
                current_slide->in_transit = true;
                break;
            case Substate::Z_UP_1:
            case Substate::Z_UP_2:
                z_axis.set_position(ZMovement::TOP);
                break;
            default:
                break;
            }

            if (substate == Substate::FINISHED) {
                state = State::IDLE;
                current_slide = nullptr;
            }
            else {
                // Move to next substate
                next_substate = static_cast<Substate>(static_cast<uint8_t>(substate) + 1);
            }
            break;
        // Idle state
        // Search for any slides that need moving, and have an open slot to be moved into
        // If found, enter MOVE_SLIDE with that slide
        // If no slides need moving for the next while, and the system has not been calibrated in a while, enter CALIBRATION
        case State::IDLE: {
            stepper_enable = true;
            Slide *next = nullptr;
            // Find a slide to move
            // Note newly inserted slides will always be at the end of the queue
            for (auto &slide : slides) {
                // Find the slide with the least amount of time remaining
                if (slide.can_move_to_next() && slide.stage_time_remaining(time) < (next ? next->stage_time_remaining(time) : INT_MAX)) {
                    next = &slide;
                }
            }
            // If this slide needs to be moved, then move it
            if (next && next->stage_time_remaining(time) <= 0) {
                state = State::MOVE_SLIDE;
                substate = Substate::X_MOVE_1;
                next_substate = Substate::X_MOVE_1;
                current_slide = next;
            }
            // If calibration period met, and the next slide that needs to be moved is more than 10s in the future, recalibrate
            else if (time - last_calibrated > hwconf::calib_period && (!next || next->stage_time_remaining(time) > 10000)) {
                state = State::CALIBRATION;
                calib_substate = CalibrationSubstate::X;
                next_calib_substate = CalibrationSubstate::X;
            }
            // When switching out of idle, re-enable all steppers
            if (state != State::IDLE) {
                stepper_enable = false;
            }
            break;
        }
        case State::CALIBRATION:
            calib_substate = next_calib_substate;
            switch (calib_substate) {
            case CalibrationSubstate::X:
                x_axis.calibrate();
                break;
            case CalibrationSubstate::Z:
                z_axis.calibrate();
                break;
            case CalibrationSubstate::RETURN:
                z_axis.set_position(ZMovement::TOP);
                break;
            case CalibrationSubstate::FINISHED:
                state = State::IDLE;
                last_calibrated = time;
                break;
            }
            if (calib_substate != CalibrationSubstate::FINISHED) {
                // Move to next substate
                next_calib_substate = static_cast<CalibrationSubstate>(static_cast<uint8_t>(calib_substate) + 1);
            }
            break;
        }
    }

    Control control;
}
