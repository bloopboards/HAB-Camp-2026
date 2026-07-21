#include <SparkFun_SCD30_Arduino_Library.h>

/*
  This Code is intended to be used with a Arduino Mega 2560, tests have been made with Arduino Uno units and they were not successful due to lack of internal memory.
  This code uses the 5V ready Micro-SD Breakout Board+ from Adafruit, the BMP390 Temp & Pressure sensor from Adafruit, the Geiger SEN0463 from Gravity, and the GY271 Magnetometer
  In order to run this code you will need to download the libraries listed below. 
  Libraries: 
  Adafruit BMP3XX Library - install all https://github.com/adafruit/Adafruit_BMP3XX 
  SD by Arduino, SparkFun https://github.com/arduino-libraries/SD 
  DFRobot_Geiger-main https://github.com/cdjq/DFRobot_Geiger
  QMC5883L Compass - https://github.com/mprograms/QMC5883LCompass 
  Parts of their example code were used throughout creating this code and you will be able to find that on their GitHub Repositories along with their libraries.

  Wiring for the circuit is listed below:
  Power:
  5V -> +
  GND -> -

  BMP390:
  VIN -> +
  GND -> -
  SCL -> 21 (SCL)
  SDA -> 20 (SDA)

  Micro SD:
  5V -> +
  GND -> GND (Digital)
  CLK -> 52
  DO -> 50
  DI -> 51
  CS -> 10

  GEIGER:
  D1 (Black) -> -
  D2 (Red) -> +
  D3 (Green) -> 3

  GY271:
  SDA -> 20 (SDA)
  SCL -> 21 (SCL)
  GND -> -
  VCC -> 5V

  PMS003I:
  VIN -> 5V
  GND -> -
  SDA -> SDA1
  SCL -> SCL1
  NOTE THE DIFFERENT SDA AND SCL HERE ^^

  SCD30:
  SDA -> 20 (SDA)
  SDA -> 21 (SCL)
  GND -> -
  VIN -> 5V

  You will need to wire multiple wires to the pins 20 (SDA) and 21 (SCL) so you will need to wire them to a common slot on the breadboard and then connect to the pins on the Arduino.
*/

#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <DFRobot_Geiger.h>
#include <QMC5883LCompass.h>
#include "Adafruit_PM25AQI.h"
#include "Sparkfun_SCD30_Arduino_Library.h"

#define detect_pin 3
#define SEALEVELPRESSURE_HPA (1013.25)

// This is for the SD Card reader
const int CS_PIN = 10;

// Sets starting values from the SCD30
float co2 = 0;
float humidity = 0;
float outsidetemp = 0;
SCD30 airSensor;

File myFile;
QMC5883LCompass compass;
Adafruit_BMP3XX bmp;
DFRobot_Geiger geiger(detect_pin);
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

void setup() {
  // Set serial to 115200
  Serial.begin(115200);

  // Output for CS Pin for Micro SD card
  pinMode(10, OUTPUT);

  // Setup the compass 
  Wire.begin();

  compass.init();
  compass.setMode(0x01,0x00,0x00,0x00);

  // Check air sensor
  if (airSensor.begin() == false) {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }

  while (!Serial) { ; }
  Serial.println(F("Initializing SD card..."));
  // Attempt to open the SD card, if it doesnt open the text below will be presented
  if (!SD.begin(CS_PIN)) {
    Serial.println(F("Card failed, or not present"));
    while (1);
  }

  // Record what happened to the Serial
  Serial.println(F("Card initialized."));
  Serial.println(F("BMP390 + SEN0463 + GY271 + PMSA003I Test"));
  
  // Starts counting with the Geiger counter
  geiger.start();

  // Start aqi for PMSA003I, 3 options for connectivity, using I2C
    if (! aqi.begin_I2C()) {      // connect to the sensor over I2C
  //if (! aqi.begin_UART(&Serial1)) { // connect to the sensor over hardware serial
  //if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial 
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
  }
  Serial.println("PM25 found!");

  // Initialize BMP390 via I2C, if it doesnt begin the text below will be written in the serial
  if (!bmp.begin_I2C(0x77)) {
    Serial.println(F("Could not find BMP390!"));
    while (1);
  }
  
  Serial.println(F("BMP390 found!"));

  // Check that SCD30 is connected
  if (airSensor.begin() == false) {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
    ;
  }

  // Configure the sensor
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  // Open file to write the header
  myFile = SD.open("data.csv", FILE_WRITE);
  if (myFile) {
    Serial.print(F("Writing header to data.csv..."));
    myFile.println(F("Internal Temp (*C), Pressure (hPa), Approx. Altitude (meters), CPM, nSv/h, uSv/h, Mag. field X (mG), Mag. field Y (mG), Mag. field Z (mG), Heading (°), Magnitude (mG), Particles > 0.3um / 0.1L air, Particles > 0.5um / 0.1L air, Particles > 1.0um / 0.1L air, Particles > 2.5um / 0.1L air, Particles > 5.0um / 0.1L air, Particles > 10 um / 0.1L air, CO2 (ppm), External/SCD30 Temp (C), Humidity (%)"));
    myFile.close();
    Serial.println(F("Done!"));
  } else {
    Serial.println(F("Error opening data.csv during setup"));
  }
}

void loop() {

  // Check PMSA003I AQI read
  PM25_AQI_Data data;
  if (! aqi.read(&data)) {
    Serial.println("Could not read from AQI");
    
    delay(500);  // try again in a bit!
    return;
  }
  Serial.println("AQI reading success");
  
  // Tell the BMP390 sensor to take a reading
  if (!bmp.performReading()) {
    Serial.println(F("Failed to perform BMP390 reading :("));
    delay(2000);
    return; // Skip this loop iteration if the sensor fails
  }

  compass.read();

  // Read the values into local variables once
  // BMP390
  float temp     = bmp.temperature;
  float pressure = bmp.pressure / 100.0;
  float altitude = bmp.readAltitude(SEALEVELPRESSURE_HPA);
  
  // SEN0463
  float cpm   = geiger.getCPM();
  float nSvh  = geiger.getnSvh();
  float uSvh  = geiger.getuSvh();

  // GY271
  int x = compass.getX();
  int y = compass.getY();
  int z = compass.getZ();
  int heading = compass.getAzimuth();
  float magnitude = sqrt(pow(x,2)+pow(y,2)+pow(z,2));

  // PMSA003I does not need ints

  // SCD30 only gets data every two seconds, so this makes sure there is data to print
  // If no data is available, it will print whatever the last data available was
  if (airSensor.dataAvailable()) {
    co2 = airSensor.getCO2();
    outsidetemp = airSensor.getTemperature();
    humidity = airSensor.getHumidity();
  }
  else {
  }

  // Print values cleanly to the Serial Monitor
  Serial.print(F("Data Line -> "));

  // BMP390
  Serial.print(temp);     Serial.print(F(", "));
  Serial.print(pressure); Serial.print(F(", "));
  Serial.print(altitude); Serial.print(F(", "));

  // SEN0463 (Geiger)
  Serial.print(cpm);      Serial.print(F(", "));
  Serial.print(nSvh);     Serial.print(F(", "));
  Serial.print(uSvh);     Serial.print(F(", "));

  // GY271 (Magnetometer + Compass)
  Serial.print(x/120);    Serial.print(F(", "));
  Serial.print(y/120);    Serial.print(F(", "));
  Serial.print(z/120);    Serial.print(F(", "));
  Serial.print(heading);  Serial.print(F(", "));
  Serial.print(magnitude/120); Serial.print(F(", "));

  // PMSA003I (Particulate)
  Serial.print(data.particles_03um); Serial.print(F(", "));
  Serial.print(data.particles_05um); Serial.print(F(", "));
  Serial.print(data.particles_10um); Serial.print(F(", "));
  Serial.print(data.particles_25um); Serial.print(F(", "));
  Serial.print(data.particles_50um); Serial.print(F(", "));
  Serial.print(data.particles_100um); Serial.print(F(", "));
  
  // SCD30 (CO2 + Humidity)
  Serial.print(co2); Serial.print(F(", "));
  Serial.print(outsidetemp); Serial.print(F(", "));
  Serial.print(humidity);

  Serial.println();

  // Open the file once and log the data
  myFile = SD.open("data.csv", FILE_WRITE);
  if (myFile) {
    // BMP390
    myFile.print(temp);     myFile.print(",");
    myFile.print(pressure); myFile.print(",");
    myFile.print(altitude); myFile.print(",");

    // SEN0463 (Geiger Counter)
    myFile.print(cpm);      myFile.print(",");
    myFile.print(nSvh);     myFile.print(",");
    myFile.print(uSvh);     myFile.print(",");

    // GY-271 (Magnetometer + Compass)
    myFile.print(x/120);    myFile.print(",");
    myFile.print(y/120);    myFile.print(",");
    myFile.print(z/120);    myFile.print(",");
    myFile.print(heading);  myFile.print(",");
    myFile.print(magnitude/120); myFile.print(",");
    
    // PMSA003I (Particulate)
    myFile.print(data.particles_03um); myFile.print(",");
    myFile.print(data.particles_05um); myFile.print(",");
    myFile.print(data.particles_10um); myFile.print(",");
    myFile.print(data.particles_25um); myFile.print(",");
    myFile.print(data.particles_50um); myFile.print(",");
    myFile.print(data.particles_100um); myFile.print(",");

    // SCD30 (CO2 + External temperature + Humidity)
    myFile.print(co2);      myFile.print(",");
    myFile.print(outsidetemp); myFile.print(",");
    myFile.print(humidity);
    myFile.println();
    
    // use println for the last item to create a new row

    myFile.close();     // Automatically flushes and saves data safely
  } else {
    Serial.println(F("Error opening data.csv for loop writing"));
  }
  // Wait 5 seconds (5000 milliseconds) before recording next interval
  delay(5000);
}
