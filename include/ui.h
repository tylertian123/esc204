#pragma once

#include "pico/stdlib.h"
#include "ui_hw.h"

namespace ui {
    extern hw::Button b_left, b_right, b_center;
    extern hw::LED led;

    void init();
}
