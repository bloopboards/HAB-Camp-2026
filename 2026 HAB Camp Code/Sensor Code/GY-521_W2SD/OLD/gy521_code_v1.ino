// hardware might be broken
// thread/conversations below the guide used, did not yield successful fixes

#include <Wire.h>

const int MPU = 0x68;
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c = 0;

void setup() {
  Serial.begin(19200);
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);    
  Wire.endTransmission(true);
  
  /*
  //Configure Accelerometer Sensitivity - Full Scale Range (depault +/- 2g)
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);
  Wire.write(0x10);
  Wire.endTransmission(true);
  //Configure Gyro Sensitivity - Full Scale Range (default +/- 250 deg/s)
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);           //Talk to the GYRO CONFIG register (1B hex)
  Wire.write(0x10);           //Set the register bits as 00010000 (1000deg/s full)
  Wire.endTransmission(true);
  delay(20);
  */

  // Call this function if you need to get the IMU error values for your module
  calculate_IMU_error();
  delay(20);

}

void loop() {
  // === Read accelerometer data === //
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total
  //For a range of +-2g, we need to divide the raw values by 16384
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; //X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; //Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0; //Z-axis value
  //Calculating Roll and Pitch from the accelerometer data
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - 0.58;
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) *190 / PI) + 1.58;

  // === Read gyroscope data === //
  previousTime = currentTime;     // Previous time is stored before the actual time read
  currentTime = millis();         //Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; //Divide by 1000 to get seconds
  Wire.beginTransmission(MPU);
  Wire.write(0x43); //Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); //Read 4 registers total
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
  //Correct the outputs with the calculated error values
  GyroX = GyroX + 0.56; // GryoErrorX ~(-0.56)
  GyroY = GyroY - 2; // GyroErrorY ~(2)
  GyroZ = GyroZ + 0.79; // GyroErrorZ ~(-0.8)

  // Currently the raw values are in deg/s, so we need to multiply by seconds to get degrees.
  gyroAngleX= gyroAngleX + GyroX * elapsedTime; //deg/s * s = deg
  gyroAngleY = gyroAngleY + GyroY *elapsedTime;
  yaw = yaw + GyroZ * elapsedTime;

  //Complementary filter - combine accelerometer and gyro angle values
  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;

  roll = gyroAngleX;
  pitch = gyroAngleY;

  // Print the values on the serial monitor
  Serial.print(roll);
  Serial.print("/");
  Serial.print(pitch);
  Serial.print("/");
  Serial.println(yaw);

}

void calculate_IMU_error() {
  // We can call this function in the setup section to calculate the accelerometer and gyroscope error
  // Note that we should place the IMU flat in order to get the proper values
  // Read accelerometer values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    // Sum all readings
    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (AccX)/sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI));
    c++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  c = 0;
  //Read gyro values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    //Sum all readings
    GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
    c++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;
  //Print the error values on the serial monitor
  Serial.print("AccErrorX: ");
  Serial.println(AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(AccErrorY);
  Serial.print("GyroErrorX: ");
  Serial.println(GyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(GyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(GyroErrorZ);
}

