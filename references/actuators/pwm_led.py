'''
ESC204S 2023 Lab 6, Task B
Task: Use PWM to modulate the brightness of an LED.
'''
import board
import pwmio
import time

# set up LED as PWM output
# (on GPIO pin 16 for Pico, needs to be changed if using Nano)
led = pwmio.PWMOut(board.GP16, frequency=1000, duty_cycle=0)

# run PWM
while True:
    for duty in range(0,65535,50):
        # increasing duty cycle
        led.duty_cycle = duty
        time.sleep(0.001)

    for duty in range(65535,0,-50):
        # decreasing duty cycle
        led.duty_cycle = duty
        time.sleep(0.001)
