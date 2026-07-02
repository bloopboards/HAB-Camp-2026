/*
Now its time to record data on an SD card for analysis.
Because we actually want to write to the SD card replace all instances of Serial.print with myFile.print (or whatever you name your file). 
It is a good idea to also output your data to the serial monitor in the same format so you can see what is being recorded in real time.
Instead of simulated data collection we will collect real temperate data from a probe.
*/

#include <SPI.h>
#include <SD.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is connected to the Arduino digital pin 2
// This can be changed to whatever you want as long as you move the corisponding pin.
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// connect the CS pin on the SD card adapter to the digital pin listed below
int CS_PIN = 4;

File myFile;

float temp;

void setup() {
  // Open serial communications and wait for port to open: (magnifying glass button in upper right corner)
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(CS_PIN)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  //Initialize Sensor.
  sensors.begin();

  //Dont forget the column headers!


  
}

void loop() {

  sensors.requestTemperatures(); 
  temp = sensors.getTempCByIndex(0);

  //Record time and temp to your sd card




}
