#pragma once

#include "pico/stdlib.h"

#include <vector>
#include <algorithm>

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

    /// @brief Current stage that the slide is in; each one corresponds to a different rack.
    Stage stage = QUEUE;
    /// @brief Which slot in the rack the slide is in. This is used by the UI display.
    uint slot;
    /// @brief System time (ms) when the slide first started processing.
    uint32_t started;
    /// @brief System time (ms) when the slide entered the current stage.
    uint32_t stage_started;
    /// @brief Set to true when the slide is picked up.
    bool in_transit = false;

    /// @brief Create a slide object in the specified slot.
    /// @param slot Slot of the slide
    /// @param started System time (ms) when the slide first started processing; if 0, will be computed
    /// @param stage_started System time (ms) when the slide entered the current stage; if 0, will be computed
    Slide(uint slot, uint32_t started = 0, uint32_t stage_started = 0);

    /// @brief Change the position of this slide. The new slot should be empty.
    /// @param new_stage The new stage
    /// @param new_slot The new slot number
    /// @return True if move was successful (slot was empty)
    bool move(Stage new_stage, uint new_slot);

    /// @brief Return the remaining time in the current stage in ms. Will be 0 or negative if the current stage is done.
    /// @param t System time in ms; if 0, will be recomputed
    /// @return Signed time remaining in the current stage
    int32_t stage_time_remaining(int32_t t = 0) const;

    /// @brief Return the total remaining time in ms. Does not take into account time to transfer slides.
    /// @param t System time in ms; if 0, will be recomputed
    /// @return Total time remaining until slide is done
    uint32_t total_time_remaining(int32_t t = 0) const;

    /// @brief Change the position of this slide to the first open slot in the next stage.
    /// @return True if move was successful
    bool move_to_next();

    /// @brief Check if there is a slot open in the next stage that the slide could be moved to.
    /// @return True if move_to_next() will be successful
    bool can_move_to_next() const;

    /// @brief Return the location of the slot that this slide is placed in.
    /// @return The horizontal location of the slot in mm.
    uint get_slot_position() const;

    /// @brief Lookup the stage name of the current stage.
    /// @return The name of the current stage.
    const char* get_stage_name() const;

    /// @brief Reset the timer on the current slide (sets stage_started to current time).
    /// @param t System time in ms; if 0, will be recomputed.
    void reset_timer(uint32_t t = 0);

    static constexpr uint STAGE_COUNT = Stage::READY + 1;
    // NOTE: These were updated for testing/demonstration.
    /// @brief The length of each stage, in milliseconds.
    static constexpr uint32_t STAGE_LEN[STAGE_COUNT] = {
        0,      // QUEUE
        5000,   // MEOH (5s)
        10000,  // MEOH_DRY (10s)
        30000, // STAIN (30s)
        10000,  // WASH (10s)
        10000,  // WASH_DRY (10s)
        0       // READY
    };

    // TODO: Change me!
    /// @brief Number of slots available for each stage.
    static constexpr uint SLOT_COUNTS[STAGE_COUNT] = {
        2,  // QUEUE
        1,  // MEOH
        2,  // MEOH_DRY
        2,  // STAIN
        1,  // WASH
        2,  // WASH_DRY
        4,  // READY
    };

    // TODO: Change me!
    /// @brief Horizontal position (mm) of each slot.
    static constexpr uint SLOT_POSITIONS[STAGE_COUNT][*std::max_element(SLOT_COUNTS, SLOT_COUNTS + STAGE_COUNT)] = {
        {10, 20},   // QUEUE
        {30},       // MEOH
        {40, 50},   // MEOH_DRY
        {60, 70},   // STAIN
        {80},       // WASH
        {90, 100},  // WASH_DRY
        {110, 120, 130, 140},  // READY
    };

    static constexpr const char *STAGE_NAMES[STAGE_COUNT] = {
        "Queue",
        "MeOH",
        "Dry 1",
        "Stain",
        "Wash",
        "Dry 2",
        "Ready",
    };

    /// @brief Current slot occupation; if a slot is occupied, the pointer will point to the slide in that slot. Otherwise nullptr.
    static Slide *slot_occupation[STAGE_COUNT][*std::max_element(SLOT_COUNTS, SLOT_COUNTS + STAGE_COUNT)];

    /// @brief Find the next available slot for the given stage.
    /// @param stage The stage to search
    /// @return Slot number of the next available slot, or -1 if none are available
    static int find_empty_slot(Stage stage);
};
