'''
ESC204S 2023 Lab 5, Task I
Task: Take readings using a microphone and plot/show result.
'''
# SPDX-FileCopyrightText: 2021 Liz Clark for Adafruit Industries
# SPDX-License-Identifier: MIT

'''Adapted from the example code DM_Mic_Sound_Level_Plot.py
https://github.com/adafruit/Adafruit_Learning_System_Guides/
blob/master/PDM_Microphone/PDM_Mic_Sound_Level_Plot.py '''

import time
import array
import math
import board
import audiobusio
import pwmio
import simpleio

def normalized_rms(values):
    """
    Helper function to get root-mean-square ("average" for continuous signal).
    """
    minbuf = int(sum(values) / len(values))
    samples_sum = sum(float(sample - minbuf) * (sample - minbuf) for sample in values)
    return math.sqrt(samples_sum / len(values))

# set analog-to-digital conversion and audio magnitude constants
ADC_LOW = 0
ADC_HIGH = 65535
MAG_LOW = 75
MAG_HIGH = 300

# IO setup
led = pwmio.PWMOut(board.LED, frequency=5000, duty_cycle=0)
mic = audiobusio.PDMIn(board.MICROPHONE_CLOCK, board.MICROPHONE_DATA, sample_rate=16000, bit_depth=16)

# continually take microphone input and send representative output as PWM signal to LED
samples = array.array('H', [0] * 160)
while True:
	# take microphone reading and get magnitude
    mic.record(samples, len(samples))
    magnitude = normalized_rms(samples)
    print((magnitude,))

	#  map mic's level to LED PWM range and send to LED
    mapped_value = simpleio.map_range(magnitude, MAG_LOW, MAG_HIGH, ADC_LOW, ADC_HIGH)
    led.duty_cycle = int(mapped_value)
    time.sleep(0.01)
