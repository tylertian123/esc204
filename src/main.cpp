#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "pico/stdlib.h"

#include "gpio.h"
#include "subsys.h"
#include "ui.h"
#include "bootsel_reset.h"

int main() {

    stdio_init_all();
    bootsel_reset::attach_timer_check();
    ui::init();

    while (true) {
        ui::poll();
        ui::update();
        subsys::control.run_once();
    }
}
