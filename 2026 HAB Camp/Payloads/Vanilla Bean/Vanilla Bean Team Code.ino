//GY271
#include <QMC5883LCompass.h>
QMC5883LCompass compass;

//UV sensor code 
 #include <SD.h>
 #include <SPI.h>
 #include <Arduino.h>
 #include <SparkFun_AS7331.h>
 #include <Wire.h>

 SfeAS7331ArdI2C myUVSensor;


//BMP390 code 
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

#define BMP_SCK 13;
#define BMP_MISO 12;
#define BMP_MOSI 11;
#define BMP_CS 10;

const int chipSelect = 10;

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP3XX bmp;

int CS_PIN = 10;
File myFile;
//

 void setup()
 {


//BMP390 code 
   Serial.begin(115200);
  while (!Serial);
  Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT);
   if (!SD.begin(CS_PIN)) {
    Serial.println(("Card failed, or not present"));
    // don't do anything more:
    while (1);
  }


  if (!bmp.begin_I2C()) {   // hardware I2C mode, can pass in address & alt Wire
  //if (! bmp.begin_SPI(BMP_CS)) {  // hardware SPI mode  
  //if (! bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) {  // software SPI mode
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    while (1);
  }

  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
  //***

  //UV sensor code 
     Wire.begin();

     // Initialize sensor and run default setup.
     myUVSensor.begin();

     Serial.println(F("Sensor began."));

     // Set measurement mode and change device operating mode to measure.
     myUVSensor.prepareMeasurement(MEAS_MODE_CMD);
     
//GY271
 compass.init();
  compass.setMode(0x01,0x00,0x00,0x00);

     //Open file print stuff 
     myFile = SD.open("data.csv", FILE_WRITE);
     myFile.println("Temperature (*C), Pressure (hPa), Approx. Altitude (meters), UVA, UVB, UVC, Magnetic field X (uT), Magnetic field Y (uT), Magnetic field Z (uT), Heading (°), Magnitude (uT)");
     myFile.close();
 }

 void loop()
 {
//BMP390
  myFile = SD.open("data.csv", FILE_WRITE);
  if (! bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  Serial.print(bmp.temperature);
  Serial.print(", ");
  
  myFile.print(bmp.temperature);
  myFile.print(", ");

  Serial.print(bmp.pressure / 100.0);
  Serial.print(", ");

  myFile.print(bmp.pressure / 100.0);
  myFile.print(", ");

  Serial.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
  myFile.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.print(", ");
  myFile.print(", ");
  //** 
  
//UV sensor code 
     // Send a start measurement command.
     if (ksfTkErrOk != myUVSensor.setStartState(true))
         Serial.println("Error starting reading!");

     // Wait for a bit longer than the conversion time.
     delay(2 + myUVSensor.getConversionTimeMillis());

     // Read UV values.
     if (ksfTkErrOk != myUVSensor.readAllUV())
         Serial.println("Error reading UV.");

     Serial.print(myUVSensor.getUVA());
     Serial.print(",");
     Serial.print(myUVSensor.getUVB());
     Serial.print(",");
     Serial.print(myUVSensor.getUVC());
     Serial.print(", ");

     myFile.print(myUVSensor.getUVA());
     myFile.print(",");
     myFile.print(myUVSensor.getUVB());
     myFile.print(",");
     myFile.print(myUVSensor.getUVC());

//GY271
compass.read();
  int x = compass.getX();
  int y = compass.getY();
  int z = compass.getZ();
  int heading = compass.getAzimuth();

float magnitude = sqrt(pow(x,2)+pow(y,2)+pow(z,2));

 Serial.print(x/120); 
  Serial.print(F(","));
  Serial.print(y/120); 
  Serial.print(F(","));
  Serial.print(z/120); 
  Serial.print(F(","));
  Serial.print(heading);
  Serial.print(F(","));
  Serial.println(magnitude/120); 
  Serial.println();

  myFile.print(x/120); 
  myFile.print(",");
  myFile.print(y/120); 
  myFile.print(",");
  myFile.print(z/120); 
  myFile.print(",");
  myFile.print(heading);
  myFile.print(",");
  myFile.println(magnitude/120); 

     myFile.close();
     delay(2000);

     //**
 }