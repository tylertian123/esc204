#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

#include "gpio.h"

util::GPIO led(PICO_DEFAULT_LED_PIN, GPIO_OUT);
util::GPIO button(16, GPIO_IN, true, false);

int main() {

    stdio_init_all();

    while (true) {
        printf("Hi\n");
        sleep_ms(100);
        led = bool(button);
    }
}
