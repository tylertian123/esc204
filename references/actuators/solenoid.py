'''
ESC204S 2023 Lab 6, Task G
Task: Control a solenoid.
'''
import board
import time
import digitalio

# set up pin as digital output
solenoid = digitalio.DigitalInOut(board.GP11)
solenoid.direction = digitalio.Direction.OUTPUT

# run solenoid by alternating switching on and off
# loop so the code runs continuously
while True:
    solenoid.value = True
    time.sleep(0.5)
    solenoid.value = False
    time.sleep(0.5)
