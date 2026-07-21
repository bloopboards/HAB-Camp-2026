#include "Wire.h" // This library allows you to communicate with I2C devices.
#include <SPI.h>
#include <SD.h>

const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.

int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
int16_t temperature; // variables for temperature data

char tmp_str[7]; // temporary variable used in convert function

const int chipSelect = 10;

File myFile;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  // for SD card 
  Serial.print("initializing SD card...");
  pinMode(10, OUTPUT);

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // create/open file
  myFile = SD.open("data.csv", FILE_WRITE);
  myFile.println("aX (g), aY (g), aZ (g), Temp (C), gX (deg/s), gY (deg/s), gZ (deg/s)");
  myFile.close();
}
void loop() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7*2, true); // request a total of 7*2=14 registers
  
  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  accelerometer_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  accelerometer_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  accelerometer_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  temperature = Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  gyro_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  gyro_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  gyro_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)

  /* print RAW data to SERIAL
  Serial.print("raw aX = "); Serial.print(accelerometer_x);
  Serial.print(" | raw aY = "); Serial.print(accelerometer_y);
  Serial.print(" | raw aZ = "); Serial.print(accelerometer_z);
  
  Serial.print(" | raw temp = "); Serial.print(temperature);

  Serial.print(" | raw gX = "); Serial.print(gyro_x);
  Serial.print(" | raw gY = "); Serial.print(gyro_y);
  Serial.print(" | raw gZ = "); Serial.print(gyro_z);
  Serial.println();
  */

  // calculate accelXYZ, temp, and gyroXYZ as floats
  float accel_x = (float)accelerometer_x/16384.0;
  float accel_y = (float)accelerometer_y/16384.0;
  float accel_z = (float)accelerometer_z/16384.0;

  // the following equation was taken from the documentation [MPU-6000/MPU-6050 Register Map and Description, p.30]
  float temp = ((temperature/340.00)+36.53);

  float gyro_x_real = ((gyro_x)/131.0);
  float gyro_y_real = ((gyro_y)/131.0);
  float gyro_z_real = ((gyro_z)/131.0);

  // print out data
  Serial.print("aX = "); Serial.print(accel_x);
  Serial.print(" | aY = "); Serial.print(accel_y);
  Serial.print(" | aZ = "); Serial.print(accel_z);
  
  Serial.print(" | temp = "); Serial.print(temp);

  Serial.print(" | gX = "); Serial.print(gyro_x_real);
  Serial.print(" | gY = "); Serial.print(gyro_y_real);
  Serial.print(" | gZ = "); Serial.print(gyro_z_real);
  Serial.println();
  
  // print data to file
  myFile = SD.open("data.csv", FILE_WRITE);
  myFile.print(accel_x);
  myFile.print(", ");
  myFile.print(accel_y);
  myFile.print(", ");
  myFile.print(accel_z);
  myFile.print(", ");
  myFile.print(temp);
  myFile.print(", ");
  myFile.print(gyro_x_real);
  myFile.print(", ");
  myFile.print(gyro_y_real);
  myFile.print(", ");
  myFile.println(gyro_z_real);
  myFile.close();

  // delay
  delay(1000);
}

// conversion equations: 
// acceleration(g) = (raw reading / 16384.0)
// temperature(°C) = raw reading(°C) / 340.00 +36.53
// gyroscope(°/s) = (raw reading / 131.0)

// units:
// g = gravitational acceleration
// °C = degrees celcius
// °/s = (angular) degrees per second
