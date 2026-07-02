************************************************************************
USE EXTREME CAUTION WHEN HANDLING THIS SENSOR. USES HIGH VOLTAGE. DO NOT TOUCH THE DETECTOR TUBE UNDER ANY CIRCUMSTANCES AS IT IS VERY FRAGILE.
************************************************************************

WIRING GUIDE

Sensor  ->      Board
---------------------
5v	->	5v
GND	->	GND
OUT	->	[Any analog pin]

Every time the sensor detects a particle a breif voltage spike will occur on the OUT pin. You must use analogRead to detect these spikes.

