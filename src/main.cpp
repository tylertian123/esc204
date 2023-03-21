#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "pico/stdlib.h"
#include "pico/time.h"

#include "gpio.h"
#include "subsys.h"
#include "bootsel_reset.h"

hw::GPIO led(PICO_DEFAULT_LED_PIN, GPIO_OUT);

enum class SystemStatus : uint8_t {
    CALIBRATION,
    NORMAL
} sys_status;

bool sys_status_led_cb(repeating_timer_t *rt) {
    if (sys_status == SystemStatus::CALIBRATION)
        led = !led;
    else
        led = true;
    return true;
}

subsys::ZMovement z_axis;
subsys::XMovement x_axis;
subsys::Gripper gripper;

int main() {

    stdio_init_all();
    bootsel_reset::attach_timer_check();

    // Add a timer to blink the status LED
    sys_status = SystemStatus::CALIBRATION;
    repeating_timer_t sys_status_timer;
    add_repeating_timer_ms(100, &sys_status_led_cb, nullptr, &sys_status_timer);

    // Calibration procedure
    z_axis.calibrate_blocking();
    x_axis.calibrate_blocking();

    sys_status = SystemStatus::NORMAL;
    sleep_ms(100);
    while (true) {
        tight_loop_contents();
    }
}
