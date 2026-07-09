Library:
https://github.com/tedyapo/arduino-MLX90393
Download ZIP
Sketch -> Include Library -> add ZIP library


Guides used:
https://learn.sparkfun.com/tutorials/bi-directional-logic-level-converter-hookup-guide
https://learn.sparkfun.com/tutorials/qwiic-magnetometer-mlx90393-hookup-guide/all
  https://github.com/sparkfun/Qwiic_Magnetometer_MLX90393


7/9/2026: uhhh we didn't need the level converter

Wiring Guide

****************************************************************************************************************************
THIS PART REQUIRES THE USE OF A LOGIC LEVEL CONVERTER.
CONFIRM WITH ADVISORS BEFORE APPLYING POWER TO THE CIRCUIT!
****************************************************************************************************************************

Only provide the sensor with 3.3 Volts. Runs on 3.3 Volt logic, must convert to 5 volts for the arduino to read.

The sensor will not work out of the box. It has several internal settings that must be tuned. Try changing the digital filtering setting.

The sensor data will be given as three values (x,y,z). Magnitude calculated. Writes to CSV.
