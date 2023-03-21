#include "ui.h"
#include "pinmap.h"
#include "util.h"

#include "pico/stdlib.h"
#include "pico/time.h"

namespace ui {
    hw::Button b_left(pinmap::left), b_right(pinmap::right), b_center(pinmap::center);
    hw::LED led(PICO_DEFAULT_LED_PIN, false);
    LCDdisplay disp(pinmap::lcd_d4, pinmap::lcd_d5, pinmap::lcd_d6, pinmap::lcd_d7, pinmap::lcd_rs, pinmap::lcd_en, 16, 2);

    repeating_timer_t timer;

    bool update_timer_cb(repeating_timer_t *rt) {
        uint32_t time = util::millis();
        b_left.poll(time);
        b_right.poll(time);
        b_center.poll(time);
        led.poll(time);
        return true;
    }

    void init() {
        add_repeating_timer_ms(10, &update_timer_cb, nullptr, &timer);
        disp.init();
    }
}
