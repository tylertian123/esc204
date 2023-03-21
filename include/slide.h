#pragma once

#include "pico/stdlib.h"

namespace sm {
    struct Slide {
        /// @brief Current stage that the slide is in; each one corresponds to a different rack.
        enum Stage : uint {
            QUEUE,
            MEOH,
            MEOH_DRY,
            STAIN,
            WASH,
            WASH_DRY,
            READY
        } stage;
        /// @brief Which slot in the rack the slide is in.
        uint slot;
        /// @brief System time (ms) when the slide first started processing.
        uint32_t started;
        /// @brief System time (ms) when the slide entered the current stage.
        uint32_t stage_started;

        /// @brief The length of each stage, in milliseconds.
        static constexpr uint32_t STAGE_LEN[Stage::READY + 1] = {
            0,      // QUEUE
            5000,   // MEOH (5s)
            30000,  // MEOH_DRY (30s)
            900000, // STAIN (15min)
            10000,  // WASH (10s)
            60000,  // WASH_DRY (60s)
            0       // READY
        };
    };
}
