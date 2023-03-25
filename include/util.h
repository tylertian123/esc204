#pragma once

#include "pico/stdlib.h"

namespace util {
    /// @brief Get the number of milliseconds since boot.
    /// @return The number of milliseconds since boot
    uint32_t millis();
    /// @brief Get the number of microseconds since boot
    /// @return The number of microseconds since boot
    uint64_t micros();
}
