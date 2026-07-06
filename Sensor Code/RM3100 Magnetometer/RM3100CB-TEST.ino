#include <Arduino.h> 
#include <SPI.h> 

//Register Map
#define RM3100_BIST 0x33 //Built-in self test
#define RM3100_STATUS 0x34 //Status of DRDY
#define RM3100_REVID 0x36 //I2C Revision ID
#define RM3100_POLL 0x00 //Polls for a single measurement
#define RM3100_CMM 0x01 //Begins Continuous Measurement Mode
#define RM3100_CCX1 0x04 //Cycle Count X Axis - Register 1
#define RM3100_CCX2 0x05 //Cycle Count X Axis - Register 2
#define RM3100_TMRC 0x0B //Data Rate for Continuous Measurement Mode
#define RM3100_MX1_WRITE 0x24 //X Axis Measurement 1 
#define RM3100_MX2_WRITE 0x25 //X Axis Measurement 2
#define RM3100_MX3_WRITE 0x26 //X Axis Measurement 3
#define RM3100_MY1_WRITE 0x27 //Y Axis Measurement 1
#define RM3100_MY2_WRITE 0x28 //Y Axis Measurement 2
#define RM3100_MY3_WRITE 0x29 //Y Axis Measurement 3
#define RM3100_MZ1_WRITE 0x2A //Z Axis Measurement 1
#define RM3100_MZ2_WRITE 0x2B //Z Axis Measurement 2
#define RM3100_MZ3_WRITE 0x2C //Z Axis Measurement 3
#define RM3100_MX1_READ 0xA4 //X Axis Measurement 1
#define RM3100_MX2_READ 0xA5 //X Axis Measurement 2
#define RM3100_MX3_READ 0xA6 //X Axis Measurement 3
#define RM3100_MY1_READ 0xA7 //Y Axis Measurement 1
#define RM3100_MY2_READ 0xA8 //Y Axis Measurement 2
#define RM3100_MY3_READ 0xA9 //Y Axis Measurement 3
#define RM3100_MZ1_READ 0xAA //Z Axis Measurement 1
#define RM3100_MZ2_READ 0xAB //Z Axis Measurement 2
#define RM3100_MZ3_READ 0xAC //Z Axis Measurement 3

//Pin Definitions
#define chipSelect 10 
#define POCI 11
#define PICO 12
#define CLK 13

//Options
#define CC 200 // Cycle Count

//Function Prototypes
byte readRegister(byte regAddress);
void writeRegister(byte regAddress, byte value)
void ContinousMeasurement(bool status);
byte singleMeasurement();

//set up functions
byte readRegister(byte regAddress){
  digitalWrite(chipSelect, LOW);
  SPI.transfer(regAddress | 0x80);
  byte recievedData = SPI.transfer(0x00);
  digitalWrite(chipSelect, HIGH);
  return recievedData;
}

//NOT FUNCTIONNG
//byte singleMeasurement(){
// digitalWrite(chipSelect, HIGH);
//  delay(10);
//  digitalWrite(chipSelect, LOW);
//  SPI.transfer(0x00);
//  SPI.transfer(0x70);
//  digitalWrite(chipSelect, HIGH);
}

//Runs at start
void setup() {
  pinMode(chipSelect, OUTPUT);
  pinMode(PICO, OUTPUT);
  pinMode(POCI, INPUT);
  digitalWrite(chipSelect, HIGH);
  SPI.begin(); 
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  Serial.begin(9600);
  delay(600);

  Serial.print(readRegister(RM3100_REVID),HEX);
}

//Main loop
void loop() {
  
}
