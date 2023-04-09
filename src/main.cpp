#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "pico/stdlib.h"

#include "gpio.h"
#include "subsys.h"
#include "ui.h"
#include "bootsel_reset.h"

_Noreturn void test_gripper_toggle() {
    while (true) {
        subsys::control.gripper.set(subsys::Gripper::OPEN);
        ui::disp.clear();
        ui::disp.print("OPEN");
        ui::disp.update();
        while (!ui::b_left.pressed) {
            ui::poll();
            sleep_ms(10);
        }
        ui::b_left.clear_all();

        subsys::control.gripper.set(subsys::Gripper::CLOSED);
        ui::disp.clear();
        ui::disp.print("CLOSED");
        ui::disp.update();
        while (!ui::b_left.pressed) {
            ui::poll();
            sleep_ms(10);
        }
        ui::b_left.clear_all();
    }
}

void test_z(uint cycles) {
    ui::disp.clear_buf();
    ui::disp.print("Calibrating");
    ui::disp.update();
    subsys::control.z_axis.calibrate_blocking();

    for (uint i = 0; i < cycles; i ++) {
        char buf[17];
        snprintf(buf, 17, "%d, rem: %d", i, cycles - i);
        ui::disp.clear_buf();
        ui::disp.print("Running cycle");
        ui::disp.print(buf, 0, 1);
        ui::disp.update();
        subsys::control.z_axis.set_position(subsys::ZMovement::TOP);
        while (subsys::control.z_axis.busy())
            tight_loop_contents();
        subsys::control.z_axis.set_position(subsys::ZMovement::BOTTOM);
        while (subsys::control.z_axis.busy())
            tight_loop_contents();
    }
    ui::disp.clear_buf();
    ui::disp.print("Done");
    ui::disp.update();
}

void test_x(uint cycles, float low_pos, float high_pos) {
    ui::disp.clear_buf();
    ui::disp.print("Calibrating");
    ui::disp.update();
    subsys::control.x_axis.calibrate_blocking();

    for (uint i = 0; i < cycles; i ++) {
        char buf[17];
        snprintf(buf, 17, "%d, rem: %d", i, cycles - i);
        ui::disp.clear_buf();
        ui::disp.print("Running cycle");
        ui::disp.print(buf, 0, 1);
        ui::disp.update();
        subsys::control.x_axis.set_position(high_pos);
        while (subsys::control.x_axis.busy())
            tight_loop_contents();
        subsys::control.x_axis.set_position(low_pos);
        while (subsys::control.x_axis.busy())
            tight_loop_contents();
    }
    ui::disp.clear_buf();
    ui::disp.print("Done");
    ui::disp.update();
}

void test_gripper(uint cycles) {
    for (uint i = 0; i < cycles; i ++) {
        char buf[17];
        snprintf(buf, 17, "%d, rem: %d", i, cycles - i);
        ui::disp.clear_buf();
        ui::disp.print("Running cycle");
        ui::disp.print(buf, 0, 1);
        ui::disp.update();

        subsys::control.gripper = subsys::Gripper::OPEN;
        sleep_ms(hwconf::gripper_change_duration);
        subsys::control.gripper = subsys::Gripper::CLOSED;
        sleep_ms(hwconf::gripper_change_duration);
    }
    ui::disp.clear_buf();
    ui::disp.print("Done");
    ui::disp.update();
}

int main() {

    stdio_init_all();
    bootsel_reset::attach_timer_check();
    ui::init();

    test_gripper_toggle();
    while (true) {
        tight_loop_contents();
    }
}
