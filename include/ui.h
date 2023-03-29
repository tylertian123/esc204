#pragma once

#include "pico/stdlib.h"
#include "ui_hw.h"
#include "pi-pico-LCD/lcd_display.hpp"

namespace ui {
    extern hw::Button b_left, b_right, b_center;
    extern hw::LED led;
    extern LCDdisplay raw_disp;
    extern hw::BufferedLCD disp;

    void init();
    void poll();
    void update();
}
