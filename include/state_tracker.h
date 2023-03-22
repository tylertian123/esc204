#pragma once

#include "pico/stdlib.h"

#include <vector>

/// @brief State tracker.
namespace st {
    class Slide {
    public:
        /// @brief Current stage that the slide is in; each one corresponds to a different rack.
        enum Stage : uint {
            QUEUE,
            MEOH,
            MEOH_DRY,
            STAIN,
            WASH,
            WASH_DRY,
            READY
        };
    private:
        /// @brief Current stage that the slide is in; each one corresponds to a different rack.
        Stage stage;
        /// @brief Which slot in the rack the slide is in.
        uint slot;
    public:
        /// @brief System time (ms) when the slide first started processing.
        uint32_t started;
        /// @brief System time (ms) when the slide entered the current stage.
        uint32_t stage_started;
        /// @brief Create a slide object in the specified slot.
        /// @param slot Slot of the slide
        /// @param started System time (ms) when the slide first started processing; if 0, will be computed
        /// @param stage_started System time (ms) when the slide entered the current stage; if 0, will be computed
        Slide(uint slot, uint32_t started = 0, uint32_t stage_started = 0);

        /// @brief Get the current stage that the slide is in.
        /// @return The stage that the slide is in
        Stage get_stage() const;
        /// @brief Get the slot that the slide is in.
        /// @return The slot that the slide is in
        uint get_slot() const;
        /// @brief Change the position of this slide. The new slot should be empty.
        /// @param new_stage The new stage
        /// @param new_slot The new slot number
        /// @return True if move was successful (slot was empty)
        bool move(Stage new_stage, uint new_slot);

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

        /// @brief Number of slots available for each stage. 0 indicates that the gripper should not let go of the slide.
        static constexpr uint SLOT_COUNTS[Stage::READY + 1] = {
            2,  // QUEUE
            0,  // MEOH
            2,  // MEOH_DRY
            2,  // STAIN
            0,  // WASH
            2,  // READY
        };

        // TODO: Change me!
        /// @brief Horizontal position (mm) of each slot.
        static constexpr uint SLOT_POSITIONS[Stage::READY + 1][2] = {
            {10, 20},   // QUEUE
            {30},       // MEOH
            {40, 50},   // MEOH_DRY
            {60, 70},   // STAIN
            {80, 90},   // WASH
            {100, 110}, // READY
        };

        /// @brief Current slot occupation; if a slot is occupied, it will be set to true.
        static bool slot_occupation[Stage::READY + 1][2];

        /// @brief Find the next available slot for the given stage.
        /// @param stage The stage to search
        /// @return Slot number of the next available slot, or -1 if none are available
        static int find_empty_slot(Stage stage);
    };

    class StateTracker {
    public:
        std::vector<Slide> slides;
    };
}
