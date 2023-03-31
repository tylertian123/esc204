#include "ui.h"

#include "pico/stdlib.h"
#include "pico/time.h"
#include "pi-pico-LCD/lcd_display.hpp"
#include "stdio.h"

#include "pinmap.h"
#include "util.h"
#include "subsys.h"
#include "slide.h"

namespace ui {
    hw::Button b_left(pinmap::left), b_right(pinmap::right), b_center(pinmap::center);
    hw::LED led(PICO_DEFAULT_LED_PIN, false);
    LCDdisplay raw_disp(pinmap::lcd_d4, pinmap::lcd_d5, pinmap::lcd_d6, pinmap::lcd_d7, pinmap::lcd_rs, pinmap::lcd_en, 16, 2);
    hw::BufferedLCD disp(raw_disp);

    uint slide_index = 0;

    void poll() {
        uint32_t time = util::millis();
        b_left.poll(time);
        b_right.poll(time);
        b_center.poll(time);
        led.poll(time);
    }

    void update() {
        using subsys::Control;
        using subsys::control;
        disp.clear_buf();
        
        if (control.state == Control::State::CALIBRATION) {
            disp.print("Calibrating...");
            if (control.calib_substate == Control::CalibrationSubstate::X) {
                disp.print("X Axis", 0, 1);
            }
            else if (control.calib_substate == Control::CalibrationSubstate::Z) {
                disp.print("Z Axis", 0, 1);
            }
        }
        else if (control.slides.size() == 0) {
            disp.print("System Idle");
            disp.print("Insert Slide", 0, 1);

        }
        else {
            if (slide_index >= control.slides.size()) {
                slide_index = 0;
            }
            if (b_left.pressed) {
                slide_index = slide_index ? slide_index - 1 : control.slides.size() - 1;
            }
            if (b_right.pressed) {
                slide_index = slide_index + 1 < control.slides.size() ? slide_index + 1 : 0;
            }
            const Slide &slide = control.slides[slide_index];
            char str[17];
            snprintf(str, 17, "#%02d Stage: %s", slide_index, slide.get_stage_name());
            disp.print(str);
            uint32_t time = util::millis();
            uint stage_time = (time - slide.stage_started) / 1000;
            uint total_rem = slide.total_time_remaining(time) / 1000;
            // 00:00 Rem: 00:00
            if (slide.in_transit) {
                snprintf(str, 17, "--:-- Rem: --:--");
            }
            else {
                snprintf(str, 17, "%02d:%02d Rem: %02d:%20d", stage_time / 60, stage_time % 60, total_rem / 60, total_rem % 60);
            }
            disp.print(str, 0, 1);
        }

        disp.update();
        b_left.clear_all();
        b_right.clear_all();
        b_center.clear_all();
    }

    void init() {
        raw_disp.init();
        disp.clear();
        led = true;
    }
}
