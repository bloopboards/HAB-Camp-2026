#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <QMC5883LCompass.h>

QMC5883LCompass compass;

void incrementTime(int timeMS);
void changeCycleCount(uint16_t newCC);
void writeReg(uint8_t addr, uint8_t data);
uint8_t readReg(uint8_t addr);

#define PIN_DRDY 1 //Set pin D9 to be the Data Ready Pin
#define RM_CS 10 //Chip Select (SS) is set to Pin 10
#define SD_CS 9 //Chip Select is set to pin 9

#define RM3100_REVID_REG 0x36 // Hexadecimal address for the Revid internal register
#define RM3100_POLL_REG 0x00 // Hexadecimal address for the Poll internal register
#define RM3100_CMM_REG 0x01 // Hexadecimal address for the Continuous Measurement Mode internal register
#define RM3100_STATUS_REG 0x34 // Hexadecimal address for the Status internal register
#define RM3100_CCX1_REG 0x04 // Hexadecimal address for the Cycle Count X1 internal register
#define RM3100_CCX0_REG 0x05 // Hexadecimal address for the Cycle Count X0 internal register

#define initialCC 200 // Set the cycle count
#define singleMode 0 //0 = use continuous measurement mode; 1 = use single measurement mode
#define useDRDYPin 0 //0 = not using DRDYPin ; 1 = using DRDYPin to wait for data
#define usingLED 1 //0 = not usng warning LED ; 1= using warning LED
#define measurementGap 5 //How long to wait between measurements, in seconds

uint8_t revid;
uint16_t cycleCount;
float gain;
double timeS = 0.0;
int columns = 9; //Number of Columns of Data, if you add more headers you MUST increase this value
const char* header[] = {"Time Elapsed (s),","RM3100 X component (uT),","RM3100 Y component (uT),","RM3100 Z component (uT),", "RM3100 Magnitude(uT),","GY X component (uT),","GY Y component (uT),","GY Z component (uT),", "GY Magnitude(uT),"};
bool titles = true;

void setup() {
  pinMode(PIN_DRDY, INPUT); 
  pinMode(SD_CS, OUTPUT); 
  digitalWrite(SD_CS, HIGH);
  pinMode(RM_CS, OUTPUT);
  digitalWrite(RM_CS, HIGH);


  Serial.begin(9600);

  Wire.begin();
  compass.init();
  SPI.begin(); // Initiate the SPI library

  compass.setMode(0x01,0x00,0x00,0x00);

  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));  
  delay(100);
  incrementTime(100);
  revid = readReg(RM3100_REVID_REG);

  Serial.print("REVID ID = 0x"); //REVID ID should be 0x22
  Serial.println(revid, HEX);

  changeCycleCount(initialCC);
  cycleCount = readReg(RM3100_CCX1_REG);
  cycleCount = (cycleCount << 8) | readReg(RM3100_CCX0_REG);

  Serial.print("Cycle Counts = "); //display cycle count
  Serial.println(cycleCount);

  gain = (0.3671 * (float)cycleCount) + 1.5; //linear equation to calculate the gain from cycle count

  Serial.print("Gain = "); //display gain; default gain should be around 75 for the default cycle count of 200
  Serial.println(gain);

  if (singleMode){
    //set up single measurement mode
    writeReg(RM3100_CMM_REG, 0);
    writeReg(RM3100_POLL_REG, 0x70);
  }
  else{
    // Enable transmission to take continuous measurement with Alarm functions off
    writeReg(RM3100_CMM_REG, 0x79);
  }

  Serial.println("Initializing SD Card...");
  if(!SD.begin(SD_CS)){
    Serial.print("FAILED");
    while(1);
  } else{
    Serial.print("SUCCESS");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  long rmx = 0;
  long rmy = 0;
  long rmz = 0;
  uint8_t x2,x1,x0,y2,y1,y0,z2,z1,z0;
  //wait until data is ready using 1 of two methods (chosen in options at top of code)
  if(useDRDYPin){ 
    while(digitalRead(PIN_DRDY) == LOW); //check RDRY pin
  }
  else{
    while((readReg(RM3100_STATUS_REG) & 0x80) != 0x80); //read internal status register
  }
  digitalWrite(RM_CS, LOW);
  delay(100);
  incrementTime(100);
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
  
  digitalWrite(RM_CS, HIGH);
  
  if (x2 & 0x80){
      rmx = 0xFF;
  }
  if (y2 & 0x80){
      rmy = 0xFF;
  }
  if (z2 & 0x80){
      rmz = 0xFF;
  }

  rmx = (rmx * 256 * 256 * 256) | (int32_t)(x2) * 256 * 256 | (uint16_t)(x1) * 256 | x0;
  rmy = (rmy * 256 * 256 * 256) | (int32_t)(y2) * 256 * 256 | (uint16_t)(y1) * 256 | y0;
  rmz = (rmz * 256 * 256 * 256) | (int32_t)(z2) * 256 * 256 | (uint16_t)(z1) * 256 | z0;

  double uT = sqrt(pow(((float)(rmx)/gain),2) + pow(((float)(rmy)/gain),2)+ pow(((float)(rmz)/gain),2));

  double xF = (double)(rmx)/gain;
  double yF = (double)(rmy)/gain;
  double zF = (double)(rmz)/gain;

  Serial.print("RM data in microTesla(uT):");
  Serial.print("   X:");
  Serial.print(xF);
  Serial.print("   Y:");
  Serial.print(yF);
  Serial.print("   Z:");
  Serial.print(zF);
  Serial.print("  magnitude(uT):");
  Serial.println(uT);
  Serial.println();

  compass.read();

  double gyx = compass.getX();
  double gyy = compass.getY();
  double gyz = compass.getZ();

  double gyMag = sqrt(pow(gyx,2)+pow(gyy,2)+pow(gyz,2));
  double gyxF = gyx/120;
  double gyyF = gyy/120;
  double gyzF = gyz/120;

  Serial.print("GY data in microTesla(uT):");
  Serial.print("   X:");
  Serial.print(gyxF);
  Serial.print("   Y:");
  Serial.print(gyyF);
  Serial.print("   Z:");
  Serial.print(gyzF);
  Serial.print("  magnitude(uT):");
  Serial.println(gyMag);
  Serial.println();

  double results[] = {timeS, xF, yF, zF, uT, gyxF, gyyF, gyzF, gyMag};
  digitalWrite(SD_CS, LOW);   
  File sensorData = SD.open("data.csv", FILE_WRITE);
    if(titles){
       for(int i = 0; i < columns; i++){
          if(i == (columns - 1)){
            sensorData.println(header[i]);
            titles = false;
            Serial.println("worked:)");
          }
           else{
            sensorData.print(header[i]); 
            sensorData.print(","); 
          }
       }
    }
    else{
      for(int i = 0; i < columns; i++){
        if(i == (columns - 1)){
          sensorData.println(results[i]); 
          sensorData.close();
        }
        else{
          sensorData.print(results[i]);
          sensorData.print(",");         
       }
    }
    }
  delay(measurementGap * 1000);
  incrementTime(measurementGap * 1000);
  digitalWrite(SD_CS, HIGH);
}

//addr is the 7 bit value of the register's address (without the R/W bit)
uint8_t readReg(uint8_t addr){
  uint8_t data = 0;
  digitalWrite(RM_CS, LOW);
  delay(100);
  SPI.transfer(addr | 0x80); //OR with 0x80 to make first bit(read/write bit) high for read
  data = SPI.transfer(0);
  digitalWrite(RM_CS, HIGH);
  incrementTime(100);
  return data;
}

//addr is the 7 bit (No r/w bit) value of the internal register's address, data is 8 bit data being written
void writeReg(uint8_t addr, uint8_t data){
  digitalWrite(RM_CS, LOW); 
  delay(100);
  incrementTime(100);
  SPI.transfer(addr & 0x7F); //AND with 0x7F to make first bit(read/write bit) low for write
  SPI.transfer(data);
  digitalWrite(RM_CS, HIGH);
}

//newCC is the new cycle count value (16 bits) to change the data acquisition
void changeCycleCount(uint16_t newCC){
  uint8_t CCMSB = (newCC & 0xFF00) >> 8; //get the most significant byte
  uint8_t CCLSB = newCC & 0xFF; //get the least significant byte
    
  digitalWrite(RM_CS, LOW); 
  delay(100);
  incrementTime(100);
  SPI.transfer(RM3100_CCX1_REG & 0x7F); //AND with 0x7F to make first bit(read/write bit) low for write
  SPI.transfer(CCMSB);  //write new cycle count to ccx1
  SPI.transfer(CCLSB);  //write new cycle count to ccx0
  SPI.transfer(CCMSB);  //write new cycle count to ccy1
  SPI.transfer(CCLSB);  //write new cycle count to ccy0
  SPI.transfer(CCMSB);  //write new cycle count to ccz1
  SPI.transfer(CCLSB);  //write new cycle count to ccz0
  digitalWrite(RM_CS, HIGH);
}

void incrementTime(int timeMS){
  timeS += (timeMS/1000);
}