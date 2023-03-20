#pragma once

#include "pico/stdlib.h"
#include "pico/time.h"

namespace bootsel_reset {
    /// @brief Perform a blocking check for a BOOTSEL reset.
    /// This call takes up to 500 ms. If reset is triggered, it will not return.
    void check_for_bootsel_reset();

    /// @brief Attach the timer interrupt for checking for BOOTSEL resets.
    void attach_timer_check();
    /// @brief Detach the timer interrupt for checking for BOOTSEL resets.
    void detach_timer_check();
}
