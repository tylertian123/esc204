#include "slide.h"

#include "util.h"

bool Slide::slot_occupation[Slide::READY + 1][2] = {false};

Slide::Slide(uint slot, uint32_t started, uint32_t stage_started) : slot(slot) {
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

bool Slide::done(uint32_t t) const {
    t = t ? t : util::millis();
    return t - stage_started > STAGE_LEN[stage];
}

bool Slide::move_to_next() {
    if (stage == READY)
        return false;
    int new_slot = find_empty_slot(static_cast<Slide::Stage>(stage + 1));
    if (new_slot == -1)
        return false;
    return move(static_cast<Slide::Stage>(stage + 1), new_slot);
}

uint Slide::get_slot_position() const {
    return SLOT_POSITIONS[stage][slot];
}

void Slide::reset_timer(uint32_t t = 0) {
    stage_started = t ? t : util::millis();
}
