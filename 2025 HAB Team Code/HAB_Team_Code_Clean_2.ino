#include "Adafruit_MLX90393.h"
#include <OneWire.h>
#include <SPI.h>
#include <SPI.h>
#include <SD.h>
#include <Arduino.h>
#include <SparkFun_AS7331.h>
#include <Wire.h>
#include <DallasTemperature.h>
#include "MS5611.h"

//Pin for One Wire communication with the Temperature Probe
#define ONE_WIRE_BUS 2
//Creates oneWire object
OneWire oneWire(ONE_WIRE_BUS);


// Creates file object for writing to the SD card
File csvFile;

//______Sensor Objects_______

//Magnometer
Adafruit_MLX90393 sensor = Adafruit_MLX90393();

//Pressure and temp
MS5611 MS5611(0x77);

//UV Sensor
SfeAS7331ArdI2C myUVSensor;

//Temperature Probe
DallasTemperature sensors(&oneWire);

//_________________________


//Data variables
float externalTemp;
float internalTemp;
float externalPressure;
float magX;
float magY;
float magZ;
float uvA;
float uvB;
float uvC;


void setup() {

  //Buzzer code
  pinMode(8,OUTPUT);

  //Setup Serial Communication for debugging
  Serial.begin(9600);

   while (!Serial) {
      delay(10);
  }

  //Signals Temperature sensor to become active
  sensors.begin();


  //_______Magnometer Initialization and Configuration________

  Serial.println("Starting Adafruit MLX90393 Demo");

  if (! sensor.begin_I2C()) {          // hardware I2C mode, can pass in address & alt Wire
    Serial.println("No sensor found ... check your wiring?");
    while (1) { delay(10); }
  }
  Serial.println("Found a MLX90393 sensor");

  sensor.setGain(MLX90393_GAIN_1X);
  

  // Set resolution, per axis. Aim for sensitivity of ~0.3 for all axes.
  sensor.setResolution(MLX90393_X, MLX90393_RES_17);
  sensor.setResolution(MLX90393_Y, MLX90393_RES_17);
  sensor.setResolution(MLX90393_Z, MLX90393_RES_16);

  // Set oversampling
  sensor.setOversampling(MLX90393_OSR_3);

  // Set digital filtering
  sensor.setFilter(MLX90393_FILTER_5);

  //_____________________________________________________________


  //__________UV Sensor initialization and configuration_________

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

    //_____________________________________



    //_______Pressure and temperature sensor init and config_________

    if (MS5611.begin() == true)
  {
    Serial.print("MS5611 found: ");
    Serial.println(MS5611.getAddress());
  }
  else
  {
    Serial.println("MS5611 not found. halt.");
    //  while (1);
  }
  Serial.println();
  Serial.println("Celsius\tmBar\tPascal");
  MS5611.setOversampling(OSR_ULTRA_HIGH);

  //___________________________________________

  //_______SD card init______________________

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

  //__________________________________________


}

void loop() {

//Buzzer code 
tone(8,1000);

  //External Pressure and temp

  MS5611.read();

  externalTemp = MS5611.getTemperature();
  externalPressure = MS5611.getPressure();

  //Internal Temp (For heating system)

  sensors.requestTemperatures(); 
  internalTemp = sensors.getTempCByIndex(0);
  

  //Magnotmeter
  sensors_event_t event;
  sensor.getEvent(&event);
  magX = event.magnetic.x;
  magY = event.magnetic.y;
  magZ = event.magnetic.z;
  

  //UV Sensor

  // Send a start measurement command.
    if (ksfTkErrOk != myUVSensor.setStartState(true))
        Serial.println("Error starting reading!");

    // Wait for a bit longer than the conversion time.
    delay(2 + myUVSensor.getConversionTimeMillis());

    // Read UV values.
    if (ksfTkErrOk != myUVSensor.readAllUV()) {
        Serial.println("Error reading UV.");
    }

    uvA = myUVSensor.getUVA();
    uvB = myUVSensor.getUVB();
    uvC = myUVSensor.getUVC();



    
    
    //Print to serial monitor
    
    Serial.print(millis()/1000); Serial.print(", ");
    Serial.print(internalTemp); Serial.print(", ");
    Serial.print(externalTemp); Serial.print(", ");
    Serial.print(externalPressure); Serial.print(", ");
    Serial.print(magX); Serial.print(", ");
    Serial.print(magY); Serial.print(", "); 
    Serial.print(magZ); Serial.print(", "); 
    Serial.print(uvA); Serial.print(", "); 
    Serial.print(uvB); Serial.print(", "); 
    Serial.print(uvC); Serial.print("\n"); 




    

  csvFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (csvFile) {
    Serial.print("Writing to file...");
    csvFile.print(millis()/1000); csvFile.print(", ");
    csvFile.print(internalTemp); csvFile.print(", ");
    csvFile.print(externalTemp); csvFile.print(", ");
    csvFile.print(externalPressure); csvFile.print(", ");
    csvFile.print(magX); csvFile.print(", ");
    csvFile.print(magY); csvFile.print(", ");
    csvFile.print(magZ); csvFile.print(", "); 
    csvFile.print(uvA); csvFile.print(", "); 
    csvFile.print(uvB); csvFile.print(", "); 
    csvFile.print(uvC); csvFile.print("\n"); 

    
    // close the file:
    csvFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  delay(1000);
  noTone(8);
  delay(4000);


}
