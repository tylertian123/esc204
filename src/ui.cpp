#include "ui.h"
#include "pinmap.h"
#include "util.h"

#include "pico/stdlib.h"
#include "pico/time.h"
#include "pi-pico-LCD/lcd_display.hpp"

namespace ui {
    hw::Button b_left(pinmap::left), b_right(pinmap::right), b_center(pinmap::center);
    hw::LED led(PICO_DEFAULT_LED_PIN, false);
    LCDdisplay raw_disp(pinmap::lcd_d4, pinmap::lcd_d5, pinmap::lcd_d6, pinmap::lcd_d7, pinmap::lcd_rs, pinmap::lcd_en, 16, 2);
    hw::BufferedLCD disp(raw_disp);

    void poll() {
        uint32_t time = util::millis();
        b_left.poll(time);
        b_right.poll(time);
        b_center.poll(time);
        led.poll(time);
    }

    void init() {
        raw_disp.init();
        disp.clear();
    }
}
