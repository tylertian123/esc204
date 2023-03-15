#include "pico/stdlib.h"
#include "gpio.h"

namespace util {
    GPIO::GPIO() {}

    GPIO::GPIO(uint pin, bool out) : pin(pin) {
        reinit(pin, out);
    }

    GPIO::GPIO(uint pin, bool out, bool pull_up, bool pull_down) : pin(pin) {
        GPIO(pin, out);
        set_pull(pull_up, pull_down);
    }

    void GPIO::reinit(uint pin, bool out) {
        this->pin = pin;
        gpio_init(pin);
        gpio_set_dir(pin, out);
    }

    void GPIO::reinit(bool out) {
        reinit(pin, out);
    }

    void GPIO::set_pull(bool up, bool down) {
        gpio_set_pulls(pin, up, down);
    }

    bool GPIO::operator=(bool val) {
        gpio_put(pin, val);
        return val;
    }

    GPIO::operator bool() const {
        return gpio_get(pin);
    }
}
