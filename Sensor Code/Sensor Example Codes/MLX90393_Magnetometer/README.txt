Manually Instal MLX90393 Library

1) Open Arduino IDE

2) Go to: Sketch (Located on the top left of the IDE window) -> Include Library -> Add .ZIP Library

3) Select "arduino-MLX90393-master.zip" (located in this folder)

4) Done


Wiring Guide

******************************************************************************************************************************************
******************************************************************************************************************************************
THIS PART REQUIRES THE USE OF A LOGIC LEVEL CONVERTER.
CONFIRM WITH ADVISORS BEFORE APPLYING POWER TO THE CIRCUIT!
******************************************************************************************************************************************
******************************************************************************************************************************************

Only provide the sensor with 3.3 Volts. Runs on 3.3 Volt logic, must convert to 5 volts for the arduino to read.

The sensor will not work out of the box. It has several internal settings that must be tuned. Try changing the digital filtering setting.

The sensor data will be given as three values (x,y,z). If you want the magnitude of the magnetic field you must write code that converts the x,y,z components into the magnitude.
