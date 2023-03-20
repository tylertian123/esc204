#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "pico/stdlib.h"

#include "gpio.h"
#include "subsys.h"
#include "ui.h"
#include "bootsel_reset.h"

hw::GPIO led(PICO_DEFAULT_LED_PIN, GPIO_OUT);

subsys::ZMovement z_axis;

int main() {

    stdio_init_all();
    bootsel_reset::attach_timer_check();
    ui::init();

    ui::led.blink(200, hw::LED::INDEFINITE);

    // Calibration procedure
    z_axis.set_position(subsys::ZMovement::TOP);
    while (z_axis.busy())
        tight_loop_contents();

    ui::led = true;
    while (true) {
        tight_loop_contents();
    }
}
