#include <Arduino.h>
#include <SPI.h>
#include "LPS22HB.h"

/*
Matlab -
  Request Sensor Init
  Request Pressure value
  Request Anemometer reading
  Request Cylinder Rotate
  Request Vertical Move

C++ - 
  Receive commands
    Return responses
  Sample pressure
  Sample anemometer
  

*/

const byte kSensorInit = 15;
const byte kSensorSetRate = 16;
const byte kPressureRequest = 31;
const byte kAnemometerRequest = 63;
const byte kAnemometerStreamRequest = 64;
const byte kMotorMove = 127;
const byte kResetMotorPos = 255;

union u_tag {
   byte b[4];
   float fval;
} u;

bool debugSerialRec = false;

LPS22HB tpSensors; 
float pressureValues[3] = {0.0, 0.0, 0.0};
float anemometerValue = 0.0;

byte cylStepPin = 5;
byte cylDirPin = 6;
byte vertStepPin = 7;
byte vertDirPin = 8;
byte anemometerPin = 9;

float degreesPerStep = 360.0/200.0;
float currentAngle = 0.0;
float maxAngle = 180.0;

float mmPerStep = 40.0/200.0;
float currentPos = 0.0;
float maxPos = 250.0;

byte inputButtonPin = 14;
int sampleFreq = 75; // Number of sensor readings taken per second 
bool streamAnemometerData = false;
long streamTime = 0;

// Set pin modes, initial pin values, and start serial port
void setup() {
  pinMode(cylStepPin, OUTPUT);
  pinMode(cylDirPin, OUTPUT);
  pinMode(vertStepPin, OUTPUT);
  pinMode(vertDirPin, OUTPUT);
  digitalWrite(cylStepPin, LOW);
  digitalWrite(cylDirPin, LOW);
  digitalWrite(vertStepPin, LOW);
  digitalWrite(vertDirPin, LOW);
  
  pinMode(inputButtonPin, INPUT);
  
  Serial.begin(9600);
  analogReadResolution(13);
  while (!Serial) {;} 
}

// Main loop - only use is for sampling and sending anemometer
// data for vortex shedding experiment 
void loop() {  
  if(streamAnemometerData ) {
    long stime = micros(); 
    float aTime = stime;
    long currTime = stime-1000;
    long prevTime = stime;
    float anemometer = 0.0;
    // Following loop prints anemometer data and microseconds
    // between samples.  Matlab parses the data and returns
    // the two relevant arrays.
    Serial.print("Times and Anemometer readings:// "); 
    while (streamTime * 1000000 > micros() - stime) {
      prevTime = currTime;
      currTime = micros();
      aTime = (float)(currTime - prevTime);
      anemometer = (float)analogRead(anemometerPin);
      Serial.print(aTime);
      Serial.print(" //,// ");
      Serial.print(anemometer);  
      Serial.print(" //,// ");
      delayMicroseconds(935);
    }
    Serial.println("");
    streamAnemometerData = false;
    
  }
}

// Function to print out pressure  and anemometer values.
// Called when request for pressure values OR anemometer values is sent
// from Matlab
void PrintPressureValues( ) { 
  Serial.print("Pressures (in Pa) for sensors {0} {1} {2} and Anemometer reading:// "); 
  Serial.print(pressureValues[0]);
  Serial.print(" //,// ");
  Serial.print(pressureValues[1]);
  Serial.print(" //,// ");
  Serial.print(pressureValues[2]);
  Serial.print(" //,// ");
  Serial.print(anemometerValue);
  Serial.println(" // ");
  
}

// Reads voltages for anemometer and pressure values from LPS22HB sensors
// for pressure.  Averages over the specified number.
void UpdateAndAveragePressures(int avgCount) {
  pressureValues[0] = 0.0;
  pressureValues[1] = 0.0;
  pressureValues[2] = 0.0;
  anemometerValue = 0.0;
  for (int i = 0; i < avgCount; i++) {
    anemometerValue += ((float)analogRead(anemometerPin)) / ((float)avgCount);
    for(int j = 0; j < 3; j++) { 
      pressureValues[j] += tpSensors.GetPressure(j) / (float)avgCount;
    }
    delay(1000 / sampleFreq + 1);
  }
}

// Moves vertical axis to the specified position
void MoveToPosition(float aPos) {
  digitalWrite(vertDirPin, currentPos > aPos);
  aPos = max(0.0, min(aPos, maxPos));
  int stepCount = round(abs(currentPos - aPos) / mmPerStep);
  int stepDir = (currentPos < aPos) * 2 - 1;
  float aNewPos = currentPos + (float)(stepDir * stepCount) * mmPerStep;
  /*Serial.print(" Moving ");
  Serial.print(stepCount);
  Serial.print(" steps for position ");
  Serial.print(aPos);*/
  for(int i = 0; i < stepCount; i++) {
    digitalWrite(vertStepPin, HIGH);
    delay(1);
    digitalWrite(vertStepPin, LOW);
    delay(20);
  }
  currentPos = aNewPos; //aPos;
}

// Moves cylinder to the specified angle
void MoveToAngle(float anAngle) {
  digitalWrite(cylDirPin, currentAngle > anAngle);
  anAngle = max(0.0, min(anAngle, maxAngle)); 
  int stepCount = round(abs(currentAngle - anAngle) / degreesPerStep);
  int stepDir = (currentAngle < anAngle) * 2 - 1;
  float aNewAngle = currentAngle + (float)(stepDir * stepCount) * degreesPerStep;
  /*Serial.print(" Moving ");
  Serial.print(stepCount);
  Serial.print(" steps for angle ");
  Serial.print(anAngle);*/
  for(int i = 0; i < stepCount; i++) {
    digitalWrite(cylStepPin, HIGH);
    delay(1);
    digitalWrite(cylStepPin, LOW);
    delay(20);
  }
  currentAngle = aNewAngle;
}


// Receives and processes commands from Matlab
void serialEvent() {
  // If serial data is available, go into this loop 
  while (Serial.available()) {
    // get the new byte received:
    int inByte =  Serial.read(); 
    if (inByte > -1 && debugSerialRec) {
      Serial.print("Rec:[");
      Serial.print((int)inByte);
      Serial.print(",");
    }
    switch ((byte)inByte) {
      case kSensorInit:
      {
        tpSensors.Begin(); 
        for(int i = 0; i < 3; i++) { 
          Serial.print(" Sensor " );
          Serial.print(i);
          Serial.print(" Reference Pressure initialized to:// " );
          Serial.print(tpSensors.GetReferencePressure(i));
          Serial.print(" // Pascals (Pa) at Temperature (C): //" );
          Serial.print(tpSensors.GetTemperature(i)); 
          Serial.print("//");
        }
        Serial.println("");
        break;
      }
      case kSensorSetRate:
      {
        inByte == -1;
        do {
          inByte = Serial.read(); 
          if (inByte > -1&& debugSerialRec) { 
            Serial.print((int)inByte);
            Serial.print(" //");
          }
        } while (inByte == -1);
        tpSensors.SetSamplesPerSecond(inByte); 
        tpSensors.writeSettings();
        Serial.print("Sample Rate Set To: // ");
        Serial.print(tpSensors.GetSamplesPerSecond()); 
        Serial.println("");
        break;
      }
      case kPressureRequest:
      {
        do {
          inByte = Serial.read(); 
          if (inByte > -1&& debugSerialRec) { 
            Serial.print((int)inByte);
            Serial.print(", ");
          }
        } while (inByte == -1);
        UpdateAndAveragePressures((byte)inByte); //sampleAverageCount);
        PrintPressureValues();
        break;
      }
      case kAnemometerRequest: 
      {
        do {
          inByte = Serial.read(); 
          if (inByte > -1&& debugSerialRec) { 
            Serial.print((int)inByte);
            Serial.print(", ");
          }
        } while (inByte == -1);
        UpdateAndAveragePressures((byte)inByte); //sampleAverageCount);
        PrintPressureValues();
        break;
      }
      case kAnemometerStreamRequest:
      {
        do {
          inByte = Serial.read(); 
          if (inByte > -1&& debugSerialRec) { 
            Serial.print((int)inByte);
            Serial.print(", ");
          }
        } while (inByte == -1);
        streamTime = (long)inByte;
        streamAnemometerData = true;
        break;
      }
      case kMotorMove:
      {
        int bytesRead = 0;
        //Serial.print("initial Bytes ");
        do {
          inByte = Serial.read(); 
          if (inByte > -1&& debugSerialRec) { 
            Serial.print((int)inByte);
            Serial.print(", ");
          }
        } while (inByte == -1);
        int targetMotor = inByte;
        /*Serial.print(targetMotor);
        Serial.print(", ");//*/
        do {
          do {
            inByte = Serial.read(); 
            if (inByte > -1&& debugSerialRec) { 
              Serial.print((int)inByte);
              Serial.print(", ");
            }
          } while (inByte == -1);
          
          /*Serial.print(inByte);
          Serial.print(", ");//*/
          u.b[bytesRead] = (byte)inByte;
          bytesRead++;
        } while (bytesRead < 4);
        
        float targetPos = u.fval;
        if (targetMotor == 0) {
          /*Serial.print("Moving Vertically...");
          Serial.print("To target " );
          Serial.print(targetPos);
          Serial.print("...");//*/
          MoveToPosition(targetPos);
          Serial.print("Moved to position: ");
          Serial.print(currentPos);
          Serial.print("  ");
        }
        else {
          /*Serial.print("Rotating Cylinder...");
          Serial.print("To target " );
          Serial.print(targetPos);
          Serial.print("...");//*/
          MoveToAngle(targetPos);
          Serial.print("Moved to angle: ");
          Serial.print(currentAngle);
          Serial.print("  ");
        }
        Serial.println("Done Moving");
        /*Serial.print("Done Moving at pos, angle = ");
        Serial.print(currentPos);
        Serial.print(", ");
        Serial.print(currentAngle);
        Serial.print(" when the target was ");
        Serial.println(targetPos);//*/
        break;
      }
      case kResetMotorPos:
        currentPos = 0.0;
        currentAngle = 0.0;
        Serial.println("Reset Pos and Angle, aka. Done Moving");
        break;
      default:

        break;
      
    }
    
  }
}


 
