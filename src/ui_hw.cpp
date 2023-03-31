#include "ui_hw.h"

#include "pi-pico-LCD/lcd_display.hpp"
#include <string.h>

#include "gpio.h"
#include "util.h"

namespace hw {
    Button::Button(uint pin) : pin(pin, GPIO_IN, true, false), down(!pin) {
        last_change = util::millis();
    }

    void Button::poll(uint32_t t) {
        t = t ? t : util::millis();
        if (in_hold) {
            held_duration = t - last_change;
        }
        // Debounce
        if (t - last_change > DEBOUNCE_DELAY) {
            bool state = !pin;
            // Check for button being held
            // Require button being previously down and a certain amount of time before last state change
            if (down && state && !in_hold && t - last_change > HOLD_DELAY) {
                held = true;
                in_hold = true;
            }
            // Check for a state change
            if (state != down) {
                // Check for button being released
                if (!state) {
                    // Only recognize it if the button is not being held down
                    if (!in_hold) {
                        pressed = true;
                    }
                    in_hold = false;
                    held_duration = 0;
                }
                down = state;
                last_change = t;
            }
        }
    }

    void Button::clear_all() {
        pressed = false;
        held = false;
    }

    Button::operator bool() const {
        return down;
    }

    LED::LED(uint pin, bool state) : pin(pin, GPIO_OUT) {
        set(state);
        last_change = util::millis();
    }

    void LED::set(bool state) {
        blink_duration = 0;
        blink_counter = 0;
        pin = state;
    }

    bool LED::blinking() const {
        return blink_duration;
    }

    void LED::blink(uint16_t duration, uint16_t times) {
        // Don't change if busy
        if (blink_duration) {
            return;
        }
        // Since the counter counts state changes, not blink cycles, this needs to be doubled
        blink_counter = times == INDEFINITE ? times : times * 2;
        blink_duration = duration;
    }

    void LED::poll(uint32_t t) {
        t = t ? t : util::millis();
        if (blink_duration) {
            // Check for state change
            if (t - last_change >= blink_duration) {
                // Add enough time for any missed changes
                last_change += blink_duration * ((t - last_change) / blink_duration);
                pin = !pin;
                // Decrement counter, stop blinking if counter reaches 0
                if (blink_counter != INDEFINITE) {
                    blink_counter --;
                    if (!blink_counter) {
                        blink_duration = 0;
                    }
                }
            }
        }
        else {
            last_change = t;
        }
    }

    bool LED::operator=(bool state) {
        set(state);
        return state;
    }

    BufferedLCD::BufferedLCD(LCDdisplay &disp) : disp(disp) {
        memset(screen, ' ', sizeof(screen));
        memset(buf, ' ', sizeof(buf));
    }

    void BufferedLCD::clear_buf() {
        memset(buf, ' ', sizeof(buf));
    }

    void BufferedLCD::clear() {
        disp.clear();
        memset(screen, ' ', sizeof(screen));
        memset(buf, ' ', sizeof(buf));
    }

    void BufferedLCD::print(const char *str, uint8_t col, uint8_t row) {
        for (uint i = 0; str[i]; i ++) {
            // Once out of bounds, stop writing
            if (col + i >= 16)
                break;
            buf[row][col + i] = str[i];
        }
    }

    bool BufferedLCD::update() {
        bool updated = false;
        for (uint row = 0; row < 2; row ++) {
            for (uint col = 0; col < 16; ) {
                // Find longest differing subsequence
                uint end;
                for (end = col; end < 16 && screen[row][end] != buf[row][end]; end ++);
                // col will be the start of the differing sequence (inclusive)
                // end will be the end of the differing sequence (exclusive)
                if (end > col) {
                    updated = true;
                    // If found, copy out the differing sequence and write to the display
                    char str_buf[17];
                    strncpy(str_buf, buf[row] + col, end - col);
                    str_buf[end - col] = '\0';
                    disp.goto_pos(col, row);
                    disp.print(str_buf);
                }
                col = end + 1;
            }
        }
        memcpy(screen, buf, sizeof(buf));
        return updated;
    }
}
