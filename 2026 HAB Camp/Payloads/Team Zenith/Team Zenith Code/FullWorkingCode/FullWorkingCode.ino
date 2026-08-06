#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <DFRobot_Geiger.h>
#include "SparkFun_TMAG5273_Arduino_Library.h"

#define detect_pin A1
#define ANALOG_PIN A0

uint8_t i2cAddress = 0x22;

#define SEALEVELPRESSURE_HPA (1013.25)

// SD card chip-select pin
const int CS_PIN = 53;

File myFile;
Adafruit_BMP3XX bmp;
TMAG5273 sensor;
DFRobot_Geiger geiger(detect_pin);

void setup() {
  delay(1000);
  Wire.begin();
  Serial.begin(9600);

  // Wait for serial, but don't hang forever if nothing's connected
  // (important for standalone/battery-powered runs)
  unsigned long serialWaitStart = millis();
  while (!Serial && (millis() - serialWaitStart < 3000)) { ; }

  Serial.println(F("Initializing SD card..."));
  pinMode(50, OUTPUT);
  pinMode(51, OUTPUT);

  if (!SD.begin(CS_PIN)) {
    Serial.println(F("Card failed, or not present"));
    while (1);
  }
  Serial.println(F("Card initialized."));

  if (sensor.begin(i2cAddress, Wire) == 1) {
    Serial.println(F("TMAG5273 begin OK"));
    sensor.setTemperatureEn(true);  // enable once here, not every loop
  } else {
    Serial.println(F("Device failed to setup - Freezing code."));
    while (1);
  }

  Serial.println(F("Adafruit BMP390 and Geiger test"));

  geiger.start();

  if (!bmp.begin_I2C(0x77)) {
    Serial.println(F("Could not find BMP390!"));
    while (1);
  }
  Serial.println(F("BMP390 found!"));

  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  // Single header row, matching the single data row we'll write each loop
  myFile = SD.open("data.csv", FILE_WRITE);
  if (myFile) {
    Serial.print(F("Writing header to data.csv..."));
    myFile.println(F("Temperature (C),Pressure (hPa),Altitude (m),CPM,nSv/h,uSv/h,"
                      "Mag X (mT),Mag Y (mT),Mag Z (mT),Magnitude (mT),Analog V"));
    myFile.close();
    Serial.println(F("Done!"));
  } else {
    Serial.println(F("Error opening data.csv during setup"));
    while (1);  // can't log without the file, so stop rather than run blind
  }
}

void loop() {
  // --- BMP390 ---
  if (!bmp.performReading()) {
    Serial.println(F("Failed to perform BMP390 reading :("));
    delay(2000);
    return;  // skip this iteration entirely, try again next loop
  }
  float temp     = bmp.temperature;
  float pressure = bmp.pressure / 100.0;
  float altitude = bmp.readAltitude(SEALEVELPRESSURE_HPA);

  // --- Geiger ---
  float cpm  = geiger.getCPM();
  float nSvh = geiger.getnSvh();
  float uSvh = geiger.getuSvh();

  // --- Magnetometer ---
  float magX = sensor.getXData();
  float magY = sensor.getYData();
  float magZ = sensor.getZData();
  float magnitude = sqrt(magX * magX + magY * magY + magZ * magZ);

  // --- UV sensor on A0 ---
  int   sensorValue   = analogRead(ANALOG_PIN);
  float sensorVoltage = sensorValue / 1024.0 * 3.3;

  // --- Print everything to Serial for live monitoring ---
  Serial.print(F("Data -> "));
  Serial.print(temp);       Serial.print(F(", "));
  Serial.print(pressure);   Serial.print(F(", "));
  Serial.print(altitude);   Serial.print(F(", "));
  Serial.print(cpm);        Serial.print(F(", "));
  Serial.print(nSvh);       Serial.print(F(", "));
  Serial.print(uSvh);       Serial.print(F(", "));
  Serial.print(magX);       Serial.print(F(", "));
  Serial.print(magY);       Serial.print(F(", "));
  Serial.print(magZ);       Serial.print(F(", "));
  Serial.print(magnitude);  Serial.print(F(", "));
  Serial.println(sensorVoltage);

  // --- Write ONE complete row per sample ---
  myFile = SD.open("data.csv", FILE_WRITE);
  if (myFile) {
    myFile.print(temp);        myFile.print(",");
    myFile.print(pressure);    myFile.print(",");
    myFile.print(altitude);    myFile.print(",");
    myFile.print(cpm);         myFile.print(",");
    myFile.print(nSvh);        myFile.print(",");
    myFile.print(uSvh);        myFile.print(",");
    myFile.print(magX);        myFile.print(",");
    myFile.print(magY);        myFile.print(",");
    myFile.print(magZ);        myFile.print(",");
    myFile.print(magnitude);   myFile.print(",");
    myFile.println(sensorVoltage);

    myFile.close();  // flushes and saves safely
  } else {
    Serial.println(F("Error opening data.csv for loop writing"));
    // Don't halt permanently on a transient write failure -- just skip
    // this row and try again next loop.
  }

  // Wait 3 seconds before the next sample
  delay(3000);
}
