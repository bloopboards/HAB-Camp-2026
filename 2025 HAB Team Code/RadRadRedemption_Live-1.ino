#include <Arduino.h>
#include <SparkFun_AS7331.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <QMC5883LCompass.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <SD.h>
#include <EEPROM.h>

//clock
int seconds = 0;

// Jacob Addition
unsigned long currentTime = 0;
unsigned long lastTime = 0;
unsigned int interval = 5000;
unsigned int EEPROM_Interval = 5*60*1000;


//---------------------------


//EEPROM vars
unsigned long lastEEPROM = 0;
int memoryIndex = 0;
int memorySize = EEPROM.length();

const int chipSelect = 8;
File RadRadPayload;

SfeAS7331ArdI2C myUVSensor;
Adafruit_BMP085 bmp;
QMC5883LCompass compass;

float x, y, z;

// Jacob modification Data varibales
int backgroundNoise;
int geigerReading;
int radiationCount = 0;

//seconds
float outsideTemp;
int pressure;
float UVA;
float UVB;
float UVC;
int EEPROM_rad_count = 0;
float mag;
float insideTemp;


int writeSpace = sizeof(UVA)*3 + sizeof(outsideTemp) + sizeof(pressure) + sizeof(mag) + sizeof(EEPROM_rad_count) + sizeof(insideTemp) + sizeof(seconds);
//----------------


#define ONE_WIRE_BUS 3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


#define ONE_WIRE_BUS 3 // DS18B20 data pin
#define HEATER_PIN 7 // Relay control pin for heater
// Temperature threshold
const float FREEZING_POINT = 10.0; // °C

void setup()
{
    compass.init();
    Serial.begin(9600);
    while (!Serial)
    {
        delay(100);
    };

    pinMode(13, OUTPUT);
    if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed");
    for (int i = 0; i <= 50; i++) {

      digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(13, LOW);
      delay(100);
        
      }
    
    
  }

  Serial.print("Write Space: "); Serial.println(writeSpace);
  Serial.print("Memory Size "); Serial.println(memorySize);




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

//BMP 180
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP180 sensor, check wiring!");
	while (1) {}
  }
//Temp Probe
sensors.begin();

//Heat Pad
pinMode(HEATER_PIN, OUTPUT);
digitalWrite(HEATER_PIN, LOW); // Heater off initially

sensors.begin();
Serial.begin(9600);

RadRadPayload = SD.open("data.csv", FILE_WRITE);

RadRadPayload.println("UVA, UVB, UVC, Temperature BMP, Pressure, Altitude (m), Pressure @ Sealevel (Pa), Real Altitude (m), Magnetic Field, Mag x, Mag y, Mag z, Rad Count, Rad Pin Background, Temperature Probe (C), Heater Status, Seconds");
RadRadPayload.close();

}
void loop(){
  currentTime = millis();

  if (currentTime - lastTime >= interval) {
    lastTime = currentTime;

//clock
seconds = currentTime/1000;

RadRadPayload = SD.open("data.csv", FILE_WRITE);

    if (RadRadPayload) { 
      Serial.println("File Loaded"); 
      } else { 
        Serial.println("File Issue");
        SD.begin(chipSelect);
        
        }

    // Send a start measurement command.
    if (ksfTkErrOk != myUVSensor.setStartState(true))
        Serial.println("Error starting reading!");

    // Wait for a bit longer than the conversion time.
    delay(2 + myUVSensor.getConversionTimeMillis());

    // Read UV values.
    if (ksfTkErrOk != myUVSensor.readAllUV())
        Serial.println("Error reading UV.");

    UVA = myUVSensor.getUVA();
    UVB = myUVSensor.getUVB();
    UVC = myUVSensor.getUVC();

    RadRadPayload.print(UVA);
    RadRadPayload.print(", ");
    RadRadPayload.print(UVB);
    RadRadPayload.print(", ");
    RadRadPayload.print(UVC);
    RadRadPayload.print(", ");

    // Serial.print("UVC: ")
    // Serial.println(myUVSensor.getUVC());

//BMP 180

    outsideTemp = bmp.readTemperature();
    pressure = bmp.readPressure();

    RadRadPayload.print(outsideTemp);
    RadRadPayload.print(", ");
    RadRadPayload.print(pressure);
    RadRadPayload.print(", ");
    RadRadPayload.print(bmp.readAltitude());
    RadRadPayload.print(", ");
    RadRadPayload.print(bmp.readSealevelPressure());
    RadRadPayload.print(", ");
    RadRadPayload.print(bmp.readAltitude(101500));
    RadRadPayload.print(", ");
    
    
//magnetic field sensor
  // Read compass values
  compass.read();

  // Return XYZ readings
  x = (float)compass.getX();
  y = (float)compass.getY();
  z = (float)compass.getZ();
  
  mag = sqrt((x*x + y*y + z*z));
  RadRadPayload.print(mag);
  RadRadPayload.print(", ");
  RadRadPayload.print(x);
  RadRadPayload.print(", ");
  RadRadPayload.print(y);
  RadRadPayload.print(", ");
  RadRadPayload.print(z);
  RadRadPayload.print(", ");
  
//Gieger
backgroundNoise = analogRead(A10);
// GeigerVolt=Geiger*(5.0/1023);

RadRadPayload.print(radiationCount);
RadRadPayload.print(", ");
RadRadPayload.print(backgroundNoise);
RadRadPayload.print(", ");

radiationCount = 0;


//Temp Probe
 sensors.requestTemperatures(); 
  insideTemp = sensors.getTempCByIndex(0);

//Heat Pad


RadRadPayload.print(insideTemp);
RadRadPayload.print(", ");

if (insideTemp < FREEZING_POINT) {
digitalWrite(HEATER_PIN, HIGH); // Turn heater on
RadRadPayload.print("ON");
RadRadPayload.print(", ");
} else {
digitalWrite(HEATER_PIN, LOW); // Turn heater off
RadRadPayload.print("OFF");
RadRadPayload.print(", ");
}
//clock
RadRadPayload.println(seconds);


RadRadPayload.close();

if ((currentTime-lastEEPROM >= EEPROM_Interval) && (memoryIndex + writeSpace < memorySize)) {
  lastEEPROM = currentTime;

  Serial.println("Writing EEPROM");

  EEPROM.put(memoryIndex, seconds);
  memoryIndex += sizeof(seconds);

  EEPROM.put(memoryIndex, outsideTemp);
  memoryIndex += sizeof(outsideTemp);

  EEPROM.put(memoryIndex, pressure);
  memoryIndex += sizeof(pressure);

  EEPROM.put(memoryIndex, UVA);
  memoryIndex += sizeof(UVA);

  EEPROM.put(memoryIndex, UVB);
  memoryIndex += sizeof(UVB);

  EEPROM.put(memoryIndex, UVC);
  memoryIndex += sizeof(UVC);

  EEPROM.put(memoryIndex, EEPROM_rad_count);
  memoryIndex += sizeof(EEPROM_rad_count);

  EEPROM.put(memoryIndex, mag);
  memoryIndex += sizeof(mag);

  EEPROM.put(memoryIndex, insideTemp);
  memoryIndex += sizeof(insideTemp);
  
  



  }

}

  



  geigerReading = analogRead(A10);
  if (geigerReading > 500) {
    radiationCount++;
     EEPROM_rad_count++;
  }



}
