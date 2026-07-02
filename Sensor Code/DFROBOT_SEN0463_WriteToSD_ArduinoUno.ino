/*!
  @file geiger.ino
  @brief    Detect CPM radiation intensity, the readings may have a large deviation at first, and the data tends to be stable after 3 times
  @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  @licence     The MIT License (MIT)
  @author [fengli](li.feng@dfrobot.com)
  @version  V1.0
  @date  2021-9-17
  @get from https://www.dfrobot.com
  @https://github.com/DFRobot/DFRobot_Geiger
*/

#include <DFRobot_Geiger.h>
#include <SPI.h>
#include <SD.h>
#if defined ESP32
#define detect_pin D3
#else
#define detect_pin 3
#endif
/*!
   @brief Constructor
   @param pin   External interrupt pin
*/

int CS_PIN = 10;
File myFile;
DFRobot_Geiger  geiger(detect_pin);

void setup()
{
  Serial.begin(115200);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT);
  if (!SD.begin(CS_PIN)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  //Start counting, enable external interrupt
  geiger.start();
}

void loop() {
  //Start counting, enable external interrupt
  //geiger.start();
  delay(3000);

  myFile = SD.open("data.txt", FILE_WRITE);

  if (myFile) {
    Serial.print("Writing to data.txt...");
    myFile.println("CPM = ");
    myFile.print(geiger.getCPM());
    myFile.println(" CPM");

    myFile.println();

    myFile.println("nSv/h = ");
    myFile.print(geiger.getnSvh());
    myFile.println(" nSv/h");

    myFile.println();
  //Pause the count, turn off the external interrupt trigger, the CPM and radiation intensity values remain in the state before the pause
  //geiger.pause();
  //Get the current CPM, if it has been paused, the CPM is the last value before the pause
  //Predict CPM by falling edge pulse within 3 seconds, the error is ±3CPM

  //Get the current nSv/h, if it has been paused, nSv/h is the last value before the pause

  // Close file
    myFile.close();
  //Get the current μSv/h, if it has been paused, the μSv/h is the last value before the pause
    Serial.println();
  }

}
