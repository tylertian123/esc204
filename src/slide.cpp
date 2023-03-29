#include "slide.h"

#include <limits.h>
#include <algorithm>

#include "util.h"

Slide *Slide::slot_occupation[Slide::READY + 1][*std::max_element(SLOT_COUNTS, SLOT_COUNTS + STAGE_COUNT)] = {nullptr};

Slide::Slide(uint slot, uint32_t started, uint32_t stage_started) : slot(slot) {
    if (!started || !stage_started) {
        uint32_t t = util::millis();
        this->started = started ? started : t;
        this->stage_started = stage_started ? stage_started : t;
    }

    slot_occupation[stage][slot] = this;
}

bool Slide::move(Slide::Stage new_stage, uint new_slot) {
    if (!slot_occupation[new_stage][new_slot]) {
        slot_occupation[new_stage][new_slot] = this;
        slot_occupation[stage][slot] = nullptr;
        stage = new_stage;
        slot = new_slot;
        return true;
    }
    return false;
}

int Slide::find_empty_slot(Slide::Stage stage) {
    for (uint i = 0; i < SLOT_COUNTS[stage]; i ++) {
        if (!slot_occupation[stage][i]) {
            return i;
        }
    }
    return -1;
}

int32_t Slide::stage_time_remaining(int32_t t) const {
    // Slides that are READY will never have a remaining time <= 0
    if (stage == READY)
        return INT_MAX;
    // Slides in QUEUE always have a remaining time of 0
    if (stage == QUEUE)
        return 0;
    t = t ? t : util::millis();
    return STAGE_LEN[stage] + stage_started - t;
}

bool Slide::move_to_next() {
    if (stage == READY)
        return false;
    int new_slot = find_empty_slot(static_cast<Slide::Stage>(stage + 1));
    if (new_slot == -1)
        return false;
    return move(static_cast<Slide::Stage>(stage + 1), new_slot);
}

bool Slide::can_move_to_next() const {
    return stage != READY && find_empty_slot(static_cast<Slide::Stage>(stage + 1)) != -1;
}

uint Slide::get_slot_position() const {
    return SLOT_POSITIONS[stage][slot];
}

const char* Slide::get_stage_name() const {
    return STAGE_NAMES[stage];
}

void Slide::reset_timer(uint32_t t) {
    stage_started = t ? t : util::millis();
}
