#include "servo.h"

#include "hardware/pwm.h"

namespace hw {
    Servo::Servo(uint pin, uint16_t period_us, uint16_t min_width_us, uint16_t max_width_us, uint range)
        : pin(pin), slice(pwm_gpio_to_slice_num(pin)), channel(pwm_gpio_to_channel(pin)), min_width(min_width_us), max_width(max_width_us), range(range) {
        gpio_set_function(pin, GPIO_FUNC_PWM);
        
        // Default sys clk is 125MHz; this makes the counter go up once every microsecond
        pwm_set_clkdiv_int_frac(slice, 125, 0);
        pwm_set_wrap(slice, period_us - 1);
        // Start at 0 duty cycle (off)
        pwm_set_chan_level(slice, channel, 0);

        pwm_set_enabled(slice, true);
    }

    void Servo::set_output(uint16_t width_us) {
        // Since clk counts once per us, just set compare level to period in us
        pwm_set_chan_level(slice, channel, width_us);
    }

    void Servo::set_angle(uint angle) {
        // Interpolate the angle
        pwm_set_chan_level(slice, channel, min_width + angle * (max_width - min_width) / range);
    }

    void Servo::stop() {
        pwm_set_chan_level(slice, channel, 0);
    }

    uint Servo::operator=(uint angle) {
        set_angle(angle);
        return angle;
    }
}
