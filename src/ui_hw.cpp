#include "ui_hw.h"

#include "gpio.h"
#include "util.h"

namespace hw {
    Button::Button(uint pin) : pin(pin, GPIO_IN, true, false) {
        last_change = util::millis();
        down = !pin;
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

    LED::LED(uint pin, bool state) : pin(pin, GPIO_OUT) {
        set(state);
        last_change = util::millis();
    }

    void LED::set(bool state) {
        blink_duration = 0;
        blink_counter = 0;
        pin = state;
    }

    bool LED::blinking() {
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
}
