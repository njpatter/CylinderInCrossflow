/* Copyright (C) 2017 Nathan J. Patterson - All Rights Reserved
 *  Unauthorized copying of this file, via any medium is strictly prohibited
 *  Contact <patterson@msoe.edu> with any questions.
 */

#define LPS22HB_cpp

#include <SPI.h>
#include "LPS22HB.h"

LPS22HB::LPS22HB() { //int dev0 , int dev1 , int dev2 , int samplesPerSecond) {
  cs[0] = 2; //dev0;
  cs[1] = 3; //dev1;
  cs[2] = 4; //dev2;

  SetSamplesPerSecond(75);
}

void LPS22HB::SetSamplesPerSecond(int samplesPerSecond) {
  switch(samplesPerSecond) {
    case 1: targetRateReg1 = reg1_1hz;
            break;
    case 10: targetRateReg1 = reg1_10hz;
            break;
    case 25: targetRateReg1 = reg1_25hz;
            break;
    case 50: targetRateReg1 = reg1_50hz;
            break;
    case 75: 
    default: targetRateReg1 = reg1_75hz;
            break; 
  }
  sampleHz = samplesPerSecond;
}

int LPS22HB::GetSamplesPerSecond() {
  byte mResp = readRegister(cs[0], CTRL_REG1);
  return mResp;
}

float LPS22HB::GetReferencePressure(byte deviceNumber) {
  lsB = readRegister(cs[deviceNumber], REF_P_XL); // LSB
  midB = readRegister(cs[deviceNumber], REF_P_L); // middle
  msB = readRegister(cs[deviceNumber], REF_P_H); // MSB 
  return ((float)(lsB | (midB<<8) | (msB<<16))) / 4096.0 * 100.0;
}

float LPS22HB::GetPressure(byte deviceNumber) {
  lsB = readRegister(cs[deviceNumber], PRESS_OUT_XL); // LSB
  midB = readRegister(cs[deviceNumber], PRESS_OUT_L); // middle
  msB = readRegister(cs[deviceNumber], PRESS_OUT_H); // MSB
  int sgnB = 0;
  if ((msB & B10000000)>>7) {
    sgnB = 0xFF;
  } 
  int reading = ((sgnB << 24) | (msB << 16) | (midB << 8) | lsB); 
  return ((float)reading / 4096.0  * 100.0);
}

float LPS22HB::GetTemperature(byte deviceNumber) {
  lsB = readRegister(cs[deviceNumber], TEMP_OUT_L); // LSB 
  msB = readRegister(cs[deviceNumber], TEMP_OUT_H); // MSB 
  return ((float)(lsB | ((msB )<<8))) / 100.0;
}

bool LPS22HB::RunIdCheck() {
  bool aVal = CheckId(0) == 177;
  aVal = aVal && CheckId(1) == 177;
  aVal = aVal && CheckId(2) == 177;
  
  return  aVal; 
}

int LPS22HB::CheckId(byte deviceNumber) {
  return readRegister(cs[deviceNumber], WHO_AM_I); 
}

void LPS22HB::Begin() { 
  pinMode(cs[0], OUTPUT);
  pinMode(cs[1], OUTPUT);
  pinMode(cs[2], OUTPUT);
  digitalWrite(cs[0], LOW);
  digitalWrite(cs[0], HIGH);
  
  digitalWrite(cs[1], LOW);
  digitalWrite(cs[1], HIGH);
  
  digitalWrite(cs[2], LOW);
  digitalWrite(cs[2], HIGH); 
  
  SPI.begin();
  while(!RunIdCheck()) {
    delay(100);
  }
  writeSettings();
}
void LPS22HB::writeSettings() { 
  for(int i = 0; i < 3; i++) {
    writeRegister(cs[i], INTERRUPT_CFG, B00010000); // Autozero reset
    writeRegister(cs[i], INTERRUPT_CFG, B00100000); // Autozero enabled
    delay(1500); // Chip needs a little time to autozero values
    writeRegister(cs[i], CTRL_REG1, targetRateReg1); // Set data update rate - 
    // (0) ( ODR2-1-0 ) (Low pass filter) (low pass config) (block data update) (SPI mode)
    // 1Hz = 001, 10 Hz = 010, 25 Hz = 011, 50 Hz = 100, 75 Hz = 101
    
    writeRegister(cs[i], CTRL_REG2, B00011000); // Auto increment enabled and I2C disabled
    writeRegister(cs[i], CTRL_REG3, B00000000);
  } 
}

int LPS22HB::writeRegister(byte deviceSsPin, byte targetRegister, byte dataToSend) {
  TransmitSpiData(deviceSsPin, targetRegister, dataToSend);
  return 0;
}

int LPS22HB::readRegister(byte deviceSsPin, byte targetRegister) {
  // Modify Register Address - First bit should be 1 if reading during second byte 
  // and second byte should be all zeros while receiving data
  byte readModRegister = targetRegister | B10000000;
  int result = TransmitSpiData(deviceSsPin, readModRegister, 0x00);
  return result;
}

int LPS22HB::TransmitSpiData(byte deviceSsPin, byte targetRegister, byte dataToSend) {
  // Specify SPI settings and begin transaction
  //SPISettings comSettings(spiRate, spiByteOrder, spiMode );
  SPISettings comSettings(4000000, MSBFIRST, SPI_MODE3);
  SPI.beginTransaction(comSettings);
  // Toggle Slave Select Low
  digitalWrite(deviceSsPin, LOW);
  // Transfer Register Address Byte
  SPI.transfer(targetRegister);
  // Write data or Read data
  int result = SPI.transfer(dataToSend);
  // Toggle Slave Select High
  digitalWrite(deviceSsPin, HIGH);
  // End SPI transaction
  SPI.endTransaction();

  return result;
}
