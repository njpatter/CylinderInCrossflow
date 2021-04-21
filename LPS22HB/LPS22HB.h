/* Copyright (C) 2017 Nathan J. Patterson - All Rights Reserved
 *  Unauthorized copying of this file, via any medium is strictly prohibited
 *  Contact <patterson@msoe.edu> with any questions.
 */

#ifndef LPS22HB_h
#define LPS22HB_h

#include <Arduino.h>
#include <SPI.h>

class LPS22HB
{
  public: 
    LPS22HB(); //int dev0, int dev1, int dev2, int samplesPerSecond );
    void Begin();
    void SetSamplesPerSecond(int samplesPerSecond);
    void writeSettings();
    bool RunIdCheck();
    float GetPressure(byte deviceNumber);
    float GetTemperature(byte deviceNumber);
    float GetReferencePressure(byte deviceNumber);
    int GetSamplesPerSecond();
    
  private:

    const int INTERRUPT_CFG = 0x0B;
    const int WHO_AM_I = 0x0F;

    const int CTRL_REG1 = 0x10;
    const int CTRL_REG2 = 0x11;
    const int CTRL_REG3 = 0x12;

    const int REF_P_XL = 0x15;
    const int REF_P_L = 0x16;
    const int REF_P_H = 0x17;

    const byte PRESS_OUT_XL = 0x28;
    const byte PRESS_OUT_L = 0x29;
    const byte PRESS_OUT_H = 0x2A;

    const byte TEMP_OUT_L = 0x2B;
    const byte TEMP_OUT_H = 0x2C;

    const byte sck = 13;  //clock - spc on device
    const byte mosi = 11; //Master Out Slave In - sdi on device
    const byte miso = 12; //Master In Slave Out - sdo on device

    byte reg1_1hz = B00010000;
    byte reg1_10hz = B00100000;
    byte reg1_25hz = B00110000;
    byte reg1_50hz = B01000000;
    byte reg1_75hz = B01010000;
    byte targetRateReg1;

    byte cs[3];
    int sampleHz = 1;
    const uint32_t  spiRate = 4000000;
    const byte spiByteOrder = MSBFIRST;
    const byte spiMode = SPI_MODE3;

    int lsB; // least significant Byte
    int midB; // middle Byte
    int msB; // most significant Byte

    int CheckId(byte deviceSsPin);
    int writeRegister(byte deviceSsPin, byte targetRegister, byte dataToSend);
    int readRegister(byte deviceSsPin, byte targetRegister);
    int TransmitSpiData(byte deviceSsPin, byte targetRegister, byte dataToSend);
};


#endif
