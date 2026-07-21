/* Test sketch for Adafruit PM2.5 sensor with UART or I2C */

#include "Adafruit_PM25AQI.h"
#include <SPI.h>
#include <SD.h>
#include <DFRobot_Geiger.h>
#if defined ESP32
#define detect_pin D3
#else
#define detect_pin 3
#endif

const int CS_PIN = 53;
File myFile;
DFRobot_Geiger  geiger(detect_pin);

// If your PM2.5 is UART only, for UNO and others (without hardware serial) 
// we must use software serial...
// pin #2 is IN from sensor (TX pin on sensor), leave pin #3 disconnected
// comment these two lines if using hardware serial
//#include <SoftwareSerial.h>
//SoftwareSerial pmSerial(2, 3);

Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

void setup() {
  // Wait for serial monitor to open
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println(F("Initializing SD card..."));
    pinMode(53, OUTPUT);
    if (!SD.begin(CS_PIN)) {
    Serial.println(F("Card failed, or not present"));
    while (1);
  }

  Serial.println(F("Card initialized."));

  Serial.println("Adafruit PMSA003I Air Quality Sensor");

  // Wait three seconds for sensor to boot up!
  delay(3000);

  // If using serial, initialize it and set baudrate before starting!
  // Uncomment one of the following
  //Serial1.begin(9600);
  //pmSerial.begin(9600);

  // There are 3 options for connectivity!
  if (! aqi.begin_I2C()) {      // connect to the sensor over I2C
  //if (! aqi.begin_UART(&Serial1)) { // connect to the sensor over hardware serial
  //if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial 
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
  }

  Serial.println("PM25 found!");

  geiger.start();

  myFile = SD.open("data.csv", FILE_WRITE);
  if (myFile) {
    Serial.print(F("Writing header to data.csv..."));
    myFile.println(F("Particles > 0.3um / 0.1L air, Particles > 0.5um / 0.1L air, Particles > 1.0um / 0.1L air, Particles > 2.5um / 0.1L air, Particles > 5.0um / 0.1L air, Particles > 10 um / 0.1L air, CPM, nSv/h, uSv/h"));
    myFile.close();
    Serial.println(F("Done!"));
  } else {
    Serial.println(F("Error opening data.csv during setup"));
}
}

void loop() {
 delay(3000)
  PM25_AQI_Data data;
  
  if (! aqi.read(&data)) {
    Serial.println("Could not read from AQI");
    
    delay(500);  // try again in a bit!
    return;
  }
  Serial.println("AQI reading success");

  Serial.println(F("---------------------------------------"));
  Serial.println(F("Concentration Units (standard)"));
  Serial.print(F("PM 1.0: ")); Serial.print(data.pm10_standard);
  Serial.print(F("\t\tPM 2.5: ")); Serial.print(data.pm25_standard);
  Serial.print(F("\t\tPM 10: ")); Serial.println(data.pm100_standard);
  Serial.println(F("---------------------------------------"));
  Serial.println(F("Concentration Units (environmental)"));
  Serial.print(F("PM 1.0: ")); Serial.print(data.pm10_env);
  Serial.print(F("\t\tPM 2.5: ")); Serial.print(data.pm25_env);
  Serial.print(F("\t\tPM 10: ")); Serial.println(data.pm100_env);
  Serial.println(F("---------------------------------------"));
  Serial.print(F("Particles > 0.3um / 0.1L air:")); Serial.println(data.particles_03um);
  Serial.print(F("Particles > 0.5um / 0.1L air:")); Serial.println(data.particles_05um);
  Serial.print(F("Particles > 1.0um / 0.1L air:")); Serial.println(data.particles_10um);
  Serial.print(F("Particles > 2.5um / 0.1L air:")); Serial.println(data.particles_25um);
  Serial.print(F("Particles > 5.0um / 0.1L air:")); Serial.println(data.particles_50um);
  Serial.print(F("Particles > 10 um / 0.1L air:")); Serial.println(data.particles_100um);
  Serial.println(F("---------------------------------------"));
  Serial.println(F("AQI"));
  Serial.print(F("PM2.5 AQI US: ")); Serial.print(data.aqi_pm25_us);
  Serial.print(F("\tPM10  AQI US: ")); Serial.println(data.aqi_pm100_us);
  Serial.print(geiger.getCPM());
  Serial.print(", ");
  Serial.print(geiger.getnSvh());
  Serial.print(", ");
  Serial.print(geiger.getuSvh());
//  Serial.print(F("PM2.5 AQI China: ")); Serial.print(data.aqi_pm25_china);
//  Serial.print(F("\tPM10  AQI China: ")); Serial.println(data.aqi_pm100_china);
  Serial.println(F("---------------------------------------"));
  Serial.println();
//Print to SD Card
 myFile = SD.open("data.csv", FILE_WRITE);
 if (myFile) {
    myFile.print(data.particles_03um);     myFile.print(",");
    myFile.print(data.particles_05um); myFile.print(",");
    myFile.print(data.particles_10um); myFile.print(",");
    myFile.print(data.particles_25um);      myFile.print(",");
    myFile.print(data.particles_50um);     myFile.print(",");
    myFile.print(data.particles_100um);   // use println for the last item to create a new row
    myFile.print(geiger.getCPM()); myFile.print(", ");
    myFile.print(geiger.getnSvh()); myFile.print(", ");
    myFile.println(geiger.getuSvh());

    myFile.close();         // Automatically flushes and saves data safely
  } else {
    Serial.println(F("Error opening data.csv for loop writing"));
  }
}
