#include <Wire.h>
#include "Adafruit_BMP3XX.h"
int heaterPin = 3;
int temp = 5;

Adafruit_BMP3XX bmp;

void setup() {
Serial.begin(115200);
Wire.begin();
pinMode(heaterPin, OUTPUT);
digitalWrite(heaterPin, LOW);
  if (!bmp.begin_I2C()) {   // hardware I2C mode, can pass in address & alt Wire
  //if (! bmp.begin_SPI(BMP_CS)) {  // hardware SPI mode  
  //if (! bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) {  // software SPI mode
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    while (1);

delay(2000);

  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);


Serial.print("starting loop.");
}
}

void loop() {

  if (! bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  float celsius = bmp.readTemperature();
 Serial.print("Celsius: ");
 Serial.println(celsius);
 if (celsius > temp) {
  digitalWrite(heaterPin, LOW);
  delay(30000); }
 else if (celsius < temp) {
  digitalWrite(heaterPin, HIGH);
  delay(30000); }
  else {
    delay(1000); 
}
  }
}
