//Forked from hnguy169 
//https://github.com/hnguy169/RM3100-Arduino/blob/main/RM3100_Arduino_SPI/RM3100_Arduino_SPI.ino

#include <Arduino.h> 
#include <SPI.h> 

//Register Map
#define RM3100_BIST 0x33 //Built-in Self test
#define RM3100_STATUS 0x34 //Status of the DRDY pin
#define RM3100_REVID 0x36 //Revision Identification
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
#define singleMode 0 //0 = use continuous measurement mode; 1 = use single measurement mode

uint8_t revid;
uint16_t cycleCount;
float gain;

//Function Prototypes
uint8_t readRegister(uint8_t regAddress);
void changeCycleCount(uint8_t newCC);
void writeRegister(uint8_t regAddress, uint8_t data);

//set up functions
uint8_t readRegister(uint8_t regAddress){
  uint8_t data = 0;
  digitalWrite(chipSelect, LOW);
  delay(100);
  SPI.transfer(regAddress | 0x80);
  data = SPI.transfer(0);
  digitalWrite(chipSelect, HIGH);
  return data;
}
void writeRegister(uint8_t regAddress, uint8_t data){
  digitalWrite(chipSelect, LOW);
  delay(100);
  SPI.transfer(regAddress & 0x7f);
  SPI.transfer(data);
  digitalWrite(chipSelect, HIGH);
}
void changeCycleCount(uint8_t newCC){
  uint8_t CCMSB = (newCC & 0xFF00) >> 8; //get the most significant byte
  uint8_t CCLSB = newCC & 0xFF; //get the least significant byte
    
  digitalWrite(chipSelect, LOW); 
  delay(100);
  SPI.transfer(RM3100_CCX2 & 0x7F); //AND with 0x7F to make first bit(read/write bit) low for write
  SPI.transfer(CCMSB);  //write new cycle count to ccx1
  SPI.transfer(CCLSB);  //write new cycle count to ccx0
  SPI.transfer(CCMSB);  //write new cycle count to ccy1
  SPI.transfer(CCLSB);  //write new cycle count to ccy0
  SPI.transfer(CCMSB);  //write new cycle count to ccz1
  SPI.transfer(CCLSB);  //write new cycle count to ccz0
  digitalWrite(chipSelect, HIGH);
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
  delay(100);

  revid = readRegister(RM3100_REVID);
  Serial.print("REVID = 0x");
  Serial.println(revid,HEX);

  changeCycleCount(CC);

  cycleCount = readRegister(RM3100_CCX2);
  cycleCount = (cycleCount << 8) | readRegister(RM3100_CCX1);

  Serial.print("Cycle Counts = ");
  Serial.println(cycleCount);

  gain = (0.3671 * (float)cycleCount) + 1.5; //linear equation to calculate the gain from cycle count

  Serial.print("Gain = "); //display gain; default gain should be around 75 for the default cycle count of 200
  Serial.println(gain);

  writeRegister(RM3100_CMM, 0x79);
}

//Main loop
void loop() {
  long x = 0;
  long y = 0;
  long z = 0;
  uint8_t x2,x1,x0,y2,y1,y0,z2,z1,z0;

  while((readRegister(RM3100_STATUS) & 0x80) != 0x80);

  digitalWrite(chipSelect, LOW);
  delay(100);
  SPI.transfer(0xA4);
  x2 = SPI.transfer(0xA5);
  x1 = SPI.transfer(0xA6);
  x0 = SPI.transfer(0xA7);
  
  y2 = SPI.transfer(0xA8);
  y1 = SPI.transfer(0xA9);
  y0 = SPI.transfer(0xAA);
  
  z2 = SPI.transfer(0xAB);
  z1 = SPI.transfer(0xAC);
  z0 = SPI.transfer(0);
  
  digitalWrite(chipSelect, HIGH);  

  if (x2 & 0x80){
      x = 0xFF;
  }
  if (y2 & 0x80){
      y = 0xFF;
  }
  if (z2 & 0x80){
      z = 0xFF;
  }

  x = (x * 256 * 256 * 256) | (int32_t)(x2) * 256 * 256 | (uint16_t)(x1) * 256 | x0;
  y = (y * 256 * 256 * 256) | (int32_t)(y2) * 256 * 256 | (uint16_t)(y1) * 256 | y0;
  z = (z * 256 * 256 * 256) | (int32_t)(z2) * 256 * 256 | (uint16_t)(z1) * 256 | z0;

  double uT = sqrt(pow(((float)(x)/gain),2) + pow(((float)(y)/gain),2)+ pow(((float)(z)/gain),2));

  Serial.print("Data in counts:");
  Serial.print("   X:");
  Serial.print(x);
  Serial.print("   Y:");
  Serial.print(y);
  Serial.print("   Z:");
  Serial.println(z);

  Serial.print("Data in microTesla(uT):");
  Serial.print("   X:");
  Serial.print((float)(x)/gain);
  Serial.print("   Y:");
  Serial.print((float)(y)/gain);
  Serial.print("   Z:");
  Serial.println((float)(z)/gain);

  //Magnitude should be around 45 uT (+/- 15 uT)
  Serial.print("Magnitude(uT):");
  Serial.println(uT);
  Serial.println(); 
}
