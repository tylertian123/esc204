'''
ESC204 2023S Lab 2 Task C
Task: Blink an external LED.
'''
# Import libraries needed for blinking the LED
import board
import digitalio
import time

# Configure the internal GPIO connected to the LED as a digital output
led = digitalio.DigitalInOut(board.GP16)
led.direction = digitalio.Direction.OUTPUT

# Print a message on the serial console
print('Hello! My LED is blinking now.')

# Loop so the code runs continuously
while True:
    led.value = True    # Turn on the LED
    time.sleep(0.5)     # wait 0.5 seconds
    led.value = False   # Turn off the LED
    time.sleep(0.5)     # wait 0.5 seconds
