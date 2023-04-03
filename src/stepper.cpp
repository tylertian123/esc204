#include "stepper.h"

#include "hardware/irq.h"
#include "hardware/pwm.h"

#include <limits.h>

namespace hw {
    
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

    A4988::A4988(uint clk, uint dir, uint32_t freq, uint8_t duty, bool invert)
        : clk(clk), slice(pwm_gpio_to_slice_num(clk)), channel(pwm_gpio_to_channel(clk)), dir_pin(dir, GPIO_OUT), inverted(invert) {
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

    A4988::A4988(uint clk, uint dir, uint32_t freq, uint8_t duty) : A4988(clk, dir, freq, duty, false) {}

    void A4988::pwm_wrap_cb_global(uint slice) {
        // Call the instance-specific callback function
        if (slice < 8 && instances[slice]) {
            instances[slice]->pwm_wrap_cb();
        }
    }

    void A4988::pwm_wrap_cb() {
        if (steps_left > 0) {
            if (steps_left != UINT_MAX && !paused)
                steps_left --;
            if (!steps_left || (has_upper_lim && direction && !upper_lim_sw) || (has_lower_lim && !direction && !lower_lim_sw)) {
                steps_left = 0;
                // Setting to a duty cycle of 0 will disable PWM
                pwm_set_chan_level(slice, channel, 0);
            }
        }
    }

    void A4988::add_lower_lim_sw(uint pin) {
        lower_lim_sw.reinit(pin, GPIO_IN);
        lower_lim_sw.set_pull(true, false);
        has_lower_lim = true;
    }

    void A4988::add_upper_lim_sw(uint pin) {
        upper_lim_sw.reinit(pin, GPIO_IN);
        upper_lim_sw.set_pull(true, false);
        has_upper_lim = true;
    }

    void A4988::step(uint num, bool dir) {
        if (num) {
            steps_left = num;
            direction = dir;
            // If inverted is true, invert the output on the direction pin
            dir_pin = inverted ? !direction : direction;
            pwm_set_chan_level(slice, channel, duty_cycle);
        }
    }

    void A4988::step(int num) {
        if (num == INT_MAX)
            step(UINT_MAX, true);
        else if (num == INT_MIN)
            step(UINT_MAX, false);
        else
            step(num > 0 ? num : -num, num > 0);
    }

    void A4988::pause() {
        pwm_set_chan_level(slice, channel, 0);
        paused = true;
    }

    void A4988::resume() {
        if (paused) {
            paused = false;
            step(steps_left, direction);
        }
    }

    void A4988::stop() {
        steps_left = 0;
        pwm_set_chan_level(slice, channel, 0);
    }

    bool A4988::done() const {
        return !steps_left;
    }
}
