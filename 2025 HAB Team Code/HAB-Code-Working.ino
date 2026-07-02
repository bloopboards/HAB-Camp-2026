#include "Adafruit_MLX90393.h"
#include <OneWire.h>

#include <SPI.h>

#include <SPI.h>
#include <SD.h>

File csvFile;


Adafruit_MLX90393 sensor = Adafruit_MLX90393();
#define MLX90393_CS 10


#include <Arduino.h>
#include <SparkFun_AS7331.h>
#include <Wire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2

SfeAS7331ArdI2C myUVSensor;


OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);


void setup() {

  Serial.begin(9600);

   while (!Serial) {
      delay(10);
  }

  sensors.begin();

  Serial.println("Starting Adafruit MLX90393 Demo");

  if (! sensor.begin_I2C()) {          // hardware I2C mode, can pass in address & alt Wire
  //if (! sensor.begin_SPI(MLX90393_CS)) {  // hardware SPI mode
    Serial.println("No sensor found ... check your wiring?");
    while (1) { delay(10); }
  }
  Serial.println("Found a MLX90393 sensor");

  sensor.setGain(MLX90393_GAIN_1X);
  // You can check the gain too
  Serial.print("Gain set to: ");
  switch (sensor.getGain()) {
    case MLX90393_GAIN_1X: Serial.println("1 x"); break;
    case MLX90393_GAIN_1_33X: Serial.println("1.33 x"); break;
    case MLX90393_GAIN_1_67X: Serial.println("1.67 x"); break;
    case MLX90393_GAIN_2X: Serial.println("2 x"); break;
    case MLX90393_GAIN_2_5X: Serial.println("2.5 x"); break;
    case MLX90393_GAIN_3X: Serial.println("3 x"); break;
    case MLX90393_GAIN_4X: Serial.println("4 x"); break;
    case MLX90393_GAIN_5X: Serial.println("5 x"); break;
  }

  // Set resolution, per axis. Aim for sensitivity of ~0.3 for all axes.
  sensor.setResolution(MLX90393_X, MLX90393_RES_17);
  sensor.setResolution(MLX90393_Y, MLX90393_RES_17);
  sensor.setResolution(MLX90393_Z, MLX90393_RES_16);

  // Set oversampling
  sensor.setOversampling(MLX90393_OSR_3);

  // Set digital filtering
  sensor.setFilter(MLX90393_FILTER_5);

  Serial.println("AS7331 UV A/B/C Command (One-shot) mode Example.");

    Wire.begin();

    // Initialize sensor and run default setup.
    if (myUVSensor.begin() == false)
    {
        Serial.println("Sensor failed to begin. Please check your wiring!");
        Serial.println("Halting...");
        while (1)
            ;
    }

    Serial.println("Sensor began.");

    // Set measurement mode and change device operating mode to measure.
    if (myUVSensor.prepareMeasurement(MEAS_MODE_CMD) == false)
    {
        Serial.println("Sensor did not get set properly.");
        Serial.println("Halting...");
        while (1)
            ;
    }

    Serial.println("Set mode to command.");



  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  csvFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (csvFile) {
    Serial.print("Writing to file...");
    csvFile.println("Time (s), Temperature (C), x (uT), y (uT), z (uT), UVA, UVB, UVC");
    // close the file:
    csvFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }


}

void loop() {


  sensors.requestTemperatures(); 
  
  Serial.print("Celsius temperature: ");
  // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  Serial.print(sensors.getTempCByIndex(0)); 
  Serial.print(" - Fahrenheit temperature: ");
  Serial.println(sensors.getTempFByIndex(0));
  delay(100);

  float x, y, z;

  // get X Y and Z data at once
  if (sensor.readData(&x, &y, &z)) {
      Serial.print("X: "); Serial.print(x, 4); Serial.println(" uT");
      Serial.print("Y: "); Serial.print(y, 4); Serial.println(" uT");
      Serial.print("Z: "); Serial.print(z, 4); Serial.println(" uT");
  } else {
      Serial.println("Unable to read XYZ data from the sensor.");
  }

  delay(100);

  /* Or....get a new sensor event, normalized to uTesla */
  sensors_event_t event;
  sensor.getEvent(&event);
  /* Display the results (magnetic field is measured in uTesla) */
  Serial.print("X: "); Serial.print(event.magnetic.x);
  Serial.print(" \tY: "); Serial.print(event.magnetic.y);
  Serial.print(" \tZ: "); Serial.print(event.magnetic.z);
  Serial.println(" uTesla ");
  
  delay(100);
  Serial.println();

  // Send a start measurement command.
    if (ksfTkErrOk != myUVSensor.setStartState(true))
        Serial.println("Error starting reading!");

    // Wait for a bit longer than the conversion time.
    delay(2 + myUVSensor.getConversionTimeMillis());

    // Read UV values.
    if (ksfTkErrOk != myUVSensor.readAllUV())
        Serial.println("Error reading UV.");

    Serial.print("UVA:");
    Serial.print(myUVSensor.getUVA());
    Serial.print(" UVB:");
    Serial.print(myUVSensor.getUVB());
    Serial.print(" UVC:");
    Serial.println(myUVSensor.getUVC());

    delay(100);
    Serial.println();

  csvFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (csvFile) {
    Serial.print("Writing to file...");
    csvFile.print(millis()/1000); csvFile.print(", ");
    csvFile.print(sensors.getTempCByIndex(0)); csvFile.print(", "); //(Time (s), Temperature (C), x (uT), y (uT), z (uT), UVA, UVB, UVC");
    csvFile.print(event.magnetic.x); csvFile.print(", "); //(Time (s), Temperature (C), x (uT), y (uT), z (uT), UVA, UVB, UVC");
    csvFile.print(event.magnetic.y); csvFile.print(", "); //(Time (s), Temperature (C), x (uT), y (uT), z (uT), UVA, UVB, UVC");
    csvFile.print(event.magnetic.z); csvFile.print(", "); //(Time (s), Temperature (C), x (uT), y (uT), z (uT), UVA, UVB, UVC");
    csvFile.print(myUVSensor.getUVA()); csvFile.print(", "); //(Time (s), Temperature (C), x (uT), y (uT), z (uT), UVA, UVB, UVC");
    csvFile.print(myUVSensor.getUVB()); csvFile.print(", "); //(Time (s), Temperature (C), x (uT), y (uT), z (uT), UVA, UVB, UVC");
    csvFile.print(myUVSensor.getUVC()); csvFile.print("\n"); //(Time (s), Temperature (C), x (uT), y (uT), z (uT), UVA, UVB, UVC");

    
    // close the file:
    csvFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }


}
