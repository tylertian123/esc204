#include "slide.h"

#include "util.h"

bool Slide::slot_occupation[Slide::READY + 1][2] = {false};

Slide::Slide(uint slot, uint32_t started, uint32_t stage_started) : stage(QUEUE), slot(slot) {
    if (!started || !stage_started) {
        uint32_t t = util::millis();
        this->started = started ? started : t;
        this->stage_started = stage_started ? stage_started : t;
    }

    slot_occupation[stage][slot] = true;
}

bool Slide::move(Slide::Stage new_stage, uint new_slot) {
    if (!slot_occupation[new_stage][new_slot]) {
        slot_occupation[new_stage][new_slot] = true;
        slot_occupation[stage][slot] = false;
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
