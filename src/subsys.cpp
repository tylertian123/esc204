#include "subsys.h"

#include "pinmap.h"
#include "stepper.h"

namespace subsys {
    ZMovement::ZMovement()
        : left_stepper(pinmap::z1_clk, pinmap::z1_dir, hwconf::z_left_freq, hwconf::z_left_duty) {
        left_stepper.add_lower_lim_sw(pinmap::z1_lower_lim);
        left_stepper.add_upper_lim_sw(pinmap::z1_upper_lim);
    }

    bool ZMovement::busy() {
        return !left_stepper.done();
    }

    ZMovement::Position ZMovement::get_position() {
        return position;
    }

    void ZMovement::set_position(Position pos) {
        if (pos != UNKNOWN) {
            left_stepper.step(pos == TOP ? INT_MAX : INT_MIN);
            position = pos;
        }
    }
}
