#pragma once

#include "pico/stdlib.h"
#include "pico/time.h"

namespace bootsel_reset {
    void check_for_bootsel_reset();

    void attach_timer_check();
    void detach_timer_check();
}
