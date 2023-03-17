#include "bootsel_reset.h"

#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/critical_section.h"
#include "hardware/structs/ioqspi.h"

// Source: @sqrtroot on GitHub
// https://gist.github.com/sqrtroot/09da3fbc5ea47504afcdc2ffb56c3694
namespace bootsel_reset {

    const uint CS_PIN_INDEX = 1;

    void set_chip_select_override(const gpio_override override) {
        hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                        override << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                        IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);
    }

    bool __no_inline_not_in_flash_func(get_bootsel_button)() {
        critical_section_t cs;
        critical_section_init(&cs);
        critical_section_enter_blocking(&cs);

        set_chip_select_override(GPIO_OVERRIDE_LOW);

        for (unsigned int i = 0; i < 1000; ++i)
            __asm("nop");

        bool button_state = !(sio_hw->gpio_hi_in & (1u << CS_PIN_INDEX));

        set_chip_select_override(GPIO_OVERRIDE_NORMAL);

        critical_section_exit(&cs);
        critical_section_deinit(&cs);

        return button_state;
    }

    void check_for_bootsel_reset() {
        if (get_bootsel_button()) {
            sleep_ms(500);
            if (get_bootsel_button()) {
                reset_usb_boot(0, 0);
            }
        }
    }

    bool bootsel_state = false;
    repeating_timer_t check_timer;

    bool timer_cb(repeating_timer_t *rt) {
        if (get_bootsel_button()) {
            if (bootsel_state) {
                reset_usb_boot(0, 0);
            }
            bootsel_state = true;
        }
        else {
            bootsel_state = false;
        }
        return true;
    }

    void attach_timer_check() {
        add_repeating_timer_ms(500, &timer_cb, nullptr, &check_timer);
    }

    void detach_timer_check() {
        cancel_repeating_timer(&check_timer);
    }
}
