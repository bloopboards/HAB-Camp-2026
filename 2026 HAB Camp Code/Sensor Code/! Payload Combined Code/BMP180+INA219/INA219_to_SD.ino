/*
  Read data from INA-219 and BMP-180, then write to SD card and serial monitor

  SD Documentation: https://www.arduino.cc/reference/en/libraries/sd/

*/
// include various libraries

#include <SPI.h>
#include <SD.h>
#include "Wire.h"
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;
File myFile;

void setup() {
  // Open serial communications and wait for port to open: (magnifying glass button in upper right corner)
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  delay(1);
  }

  Serial.print("Initializing SD card...");

//CS Pin on Adafruit SD breakout board needs to be connected to pin 53 (for arduino mega), or change this number in the next line
  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); } 
}
// To use a slightly lower 32V, 1A range (higher precision on amps):
//ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
ina219.setCalibration_16V_400mA();

  Serial.println("Measuring voltage and current with INA219 ...");

  myFile = SD.open("data.csv", FILE_WRITE);
  myFile.println("Bus Voltage (V), Shunt Voltage (mV), Load Voltage (V), Current (mA), Power (mW)");
  myFile.close();
}

void loop() {

  // set variables for INA-219 equal to zero
    float shuntvoltage = 0;
    float busvoltage = 0;
    float current_mA = 0;
    float loadvoltage = 0;
    float power_mW = 0;

  // get readings from INA-219
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);

// open the file. note that only one file can be open at a time, 
// so you have to close this one before you can open another
  myFile = SD.open("data.csv", FILE_WRITE);

  // if the file opened okay, write data to it:
  if (myFile) {
    Serial.print("Writing to data.csv...");

    myFile.print(busvoltage); 
    myFile.print(", ");
    myFile.print(shuntvoltage); 
    myFile.print(", ");
    myFile.print(loadvoltage); 
    myFile.print(", ");
    myFile.print(current_mA);
    myFile.print(", ");
    myFile.print(power_mW);
    myFile.println(", ");
    myFile.println("");

    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
    //now print data to serial monitor
    Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
    Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
    Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
    Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
    Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
    Serial.println("");
 
    Serial.println();
    delay(1000); //wait one minute
 
}