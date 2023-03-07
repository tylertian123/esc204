'''
ESC204S 2023 Lab 5, Task H
Task: Take readings using an IMU sensor.
'''
# SPDX-FileCopyrightText: Copyright (c) 2020 Bryan Siepert
# for Adafruit Industries
# SPDX-License-Identifier: MIT
import time
import board
from adafruit_lsm6ds.lsm6dsox import LSM6DSOX

# set up I2C connection to IMU
i2c = board.I2C()  # uses board.SCL and board.SDA
sensor = LSM6DSOX(i2c)

# set mappings
motion_axis = {"X":0, "Y":1, "Z":2}

# set mode options and mappings
PLOT_DATA = False   # True to plot in plotter, False to print
AXIS = "X"          # set to "X", "Y", or "Z"
TYPE = "gyro"       # set to "accel" or "gyro"

# continuously print IMU readings
while True:
    if PLOT_DATA:
        if TYPE == "accel":
            print((sensor.acceleration[motion_axis[AXIS]],))
        else:
            print((sensor.gyro[motion_axis[AXIS]],))
    else:
        print("Acceleration: X:%.2f, Y: %.2f, Z: %.2f m/s^2" % (sensor.acceleration))
        print("Gyro          X:%.2f, Y: %.2f, Z: %.2f radians/s" % (sensor.gyro))
    time.sleep(0.5)
