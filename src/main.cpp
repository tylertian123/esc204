#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "pico/stdlib.h"

#include "gpio.h"
#include "subsys.h"
#include "ui.h"
#include "bootsel_reset.h"

hw::GPIO led(PICO_DEFAULT_LED_PIN, GPIO_OUT);

subsys::Control system;

int main() {

    stdio_init_all();
    bootsel_reset::attach_timer_check();
    ui::init();

    ui::led = true;
    while (true) {
        system.run_once();
    }
}
