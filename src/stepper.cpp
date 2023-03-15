#include "stepper.h"

#include "hardware/irq.h"
#include "hardware/pwm.h"

#include <stdio.h>

namespace util {
    
    void pwm_wrap_handler() {
        uint32_t irq_status = pwm_get_irq_status_mask();
        // Use the bit in the IRQ status to determine which PWM wrap triggered
        for (uint slice = 0; slice < 8; slice ++) {
            if (irq_status & (1 << slice)) {
                A4988::pwm_wrap_cb_global(slice);
                pwm_clear_irq(slice);
            }
        }
    }

    A4988 *A4988::instances[8] = {nullptr};

    A4988::A4988(uint clk, uint dir, uint32_t freq, uint8_t duty) : clk(clk), dir_pin(dir, GPIO_OUT) {
        slice = pwm_gpio_to_slice_num(clk);
        channel = pwm_gpio_to_channel(clk);
        gpio_set_function(clk, GPIO_FUNC_PWM);

        // Default sys clk is 125MHz; this makes the counter go up once every microsecond
        pwm_set_clkdiv_int_frac(slice, 125, 0);
        // Set wrap to be period in us - 1
        uint16_t wrap = 1000000 / freq - 1;
        pwm_set_wrap(slice, wrap);
        // Set the duty cycle according to wrap
        duty_cycle = duty * wrap / 255;
        // Since the PWM is disabled at the start, set it to a duty cycle of 0
        pwm_set_chan_level(slice, channel, 0);

        // Enable interrupts for PWM wrap
        pwm_clear_irq(slice);
        pwm_set_irq_enabled(slice, true);
        irq_set_exclusive_handler(PWM_IRQ_WRAP, &pwm_wrap_handler);
        irq_set_enabled(PWM_IRQ_WRAP, true);

        instances[slice] = this;
        pwm_set_enabled(slice, true);
    }

    void A4988::pwm_wrap_cb_global(uint slice) {
        // Call the instance-specific callback function
        if (slice < 8 && instances[slice]) {
            instances[slice]->pwm_wrap_cb();
        }
    }

    void A4988::pwm_wrap_cb() {
        if (steps_left > 0) {
            steps_left --;
            if (!steps_left) {
                // Setting to a duty cycle of 0 will disable PWM
                pwm_set_chan_level(slice, channel, 0);
            }
        }
    }

    void A4988::step(uint num, bool direction) {
        if (num) {
            steps_left = num;
            dir_pin = direction;
            pwm_set_chan_level(slice, channel, duty_cycle);
        }
    }

    void A4988::stop() {
        pwm_set_chan_level(slice, channel, 0);
    }

    bool A4988::done() {
        return !steps_left;
    }
}
