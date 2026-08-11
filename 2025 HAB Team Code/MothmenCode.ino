#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include "SparkFunISL29125.h"
#include <SPI.h>
#include <SD.h>

/*
This is a compilation of code from 'RGBcode' and 'BMPcode' both of which are in the Flash drive
Seconds/ time measurement are not recorded in those separated files, but instead here.
Hopefully this will streamline data and avoid redundancy in timing- as well as maintaining uniform timestamps.
*/
   
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
int CS_PIN = 7;

File myFile;
unsigned long seconds; //convert to seconds

// Declare sensor object
SFE_ISL29125 RGB_sensor;
/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
    This is specifically the BMP one, the RGB sensor doesn't have a setup thingy.
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensor;
  bmp.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" hPa");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" hPa");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" hPa");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void) 
{
  // Initialize serial communication
  Serial.begin(9600);

    // Initialize the ISL29125 with simple configuration so it starts sampling
  if (RGB_sensor.init())
  {
    Serial.println("Sensor Initialization Successful\n\r");
  }
  // Initialize the BMP, I think
  Serial.println("Pressure Sensor Test"); Serial.println("");

  Serial.print("Initializing SD card...");

  if (!SD.begin(CS_PIN)) {
    Serial.println("SD initialization failed!");
    while (1);
  }
  Serial.println("SD initialization done.");
  
  // Initialise the sensor
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  /* Display some basic information on this sensor */
  displaySensorDetails();
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop() {

   myFile = SD.open("Mothman.txt", FILE_WRITE);
  seconds = millis()/1000;
 Serial.print("Time: ");
  Serial.println(seconds);
  myFile.print("Time: "); myFile.println(seconds);

  /* Get a new sensor event */ 
  sensors_event_t event;
  bmp.getEvent(&event);
 
  /* Display the results (barometric pressure is measure in hPa) */
  if (event.pressure)
  {
    /* Display atmospheric pressue in hPa */
    Serial.print("Pressure:    ");
    Serial.print(event.pressure);
    Serial.println(" hPa");
    
 myFile.print("Pressure:    ");
    myFile.print(event.pressure);
    myFile.println(" hPa");

    /* Calculating altitude with reasonable accuracy requires pressure    *
     * sea level pressure for your position at the moment the data is     *
     * converted, as well as the ambient temperature in degress           *
     * celcius.  If you don't have these values, a 'generic' value of     *
     * 1013.25 hPa can be used (defined as SENSORS_PRESSURE_SEALEVELHPA   *
     * in sensors.h), but this isn't ideal and will give variable         *
     * results from one day to the next.                                  *
     *                                                                    *
     * You can usually find the current SLP value by looking at weather   *
     * websites or from environmental information centers near any major  *
     * airport.                                                           *
     *                                                                    *
     * For example, for Paris, France you can check the current mean      *
     * pressure and sea level at: http://bit.ly/16Au8ol                   */
     
    /* First we get the current temperature from the BMP085 */
    float temperature;
    bmp.getTemperature(&temperature);
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");

   myFile.print("Temperature: ");
    myFile.print(temperature);
    myFile.println(" C");

    /* Then convert the atmospheric pressure, and SLP to altitude         */
    /* Update this next line with the current SLP for better results      */
    float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
    Serial.print("Altitude:    "); 
    Serial.print(bmp.pressureToAltitude(seaLevelPressure,
                                        event.pressure)); 
    Serial.println(" m");
    Serial.println("");

    myFile.print("Altitude:    "); 
    myFile.print(bmp.pressureToAltitude(seaLevelPressure,
                                        event.pressure)); 
    myFile.println(" m");
    myFile.println("");

  }
  else
  {
    Serial.println("Sensor error");
  }

  // Read RGB sensor values (16 bit integers)
  unsigned int red = RGB_sensor.readRed();
  unsigned int green = RGB_sensor.readGreen();
  unsigned int blue = RGB_sensor.readBlue();
  
  // Print out readings, change HEX to DEC if you prefer decimal output
  Serial.print("Red: "); Serial.println(red,DEC);
  Serial.print("Green: "); Serial.println(green,DEC);
  Serial.print("Blue: "); Serial.println(blue,DEC);
  Serial.println();
  myFile.print("Red: "); myFile.println(red,DEC);
  myFile.print("Green: "); myFile.println(green,DEC);
  myFile.print("Blue: "); myFile.println(blue,DEC);
  myFile.println();
  myFile.close();
  delay(1000);

  //Record time and temp to your sd card

}

