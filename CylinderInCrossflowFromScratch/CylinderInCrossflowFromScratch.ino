#include <Arduino.h>
#include <SPI.h>
#include "LPS22HB.h"

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

float angleIncrement = 3.6;
float verticalIncrement = 2.4;

byte inputButtonPin = 14;
int sampleFreq = 75; // Number of sensor readings taken per second 
int sampleCountForAvg = 50;
bool streamAnemometerData = false;
long streamTime = 0;
bool isInitialized = false;

int millisDelayAfterMoves = 3000;
int stepDelayMillis = 20; // Could be lowered to get faster movement

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
  
  // initialize sensors
  tpSensors.Begin(); 
  delay(500);
  
  // Write and then read sensor sample rate
  tpSensors.SetSamplesPerSecond(50); 
  tpSensors.writeSettings();
  Serial.print("Sample Rate Updated To (returned as hex setting from sensor, see datasheet for more info): ");
  Serial.println(tpSensors.GetSamplesPerSecond());

  
}

void loop() {  
  // Pressure sensor usage examples
  Serial.println("");
  
  // print out reference pressure and temperature from sensor i
  int i = 1;
  Serial.print("Reference pressure/temperature: ");
  Serial.print(tpSensors.GetReferencePressure(i)); 
  Serial.print(" Pascals (Pa) at Temperature : " );
  Serial.print(tpSensors.GetTemperature(i)); 
  Serial.println(" degrees C ");
  
  // Read current pressures, temperatures, and anemometer reading
  Serial.print("Pressures (in Pa) for sensors {0} {1} {2} and Anemometer reading:  "); 
  Serial.print(tpSensors.GetPressure(0));
  Serial.print(", ");
  Serial.print(tpSensors.GetPressure(1));
  Serial.print(", ");
  Serial.print(tpSensors.GetPressure(2));
  Serial.print(", ");
  Serial.print(anemometerValue);
  Serial.println("");


  delay(1000);
  
}

void HandleMenu() {

}

void RunDragExperiment() {

}

void StreamAnemometerData(int secondsToStream) {
 
}

void PrintPressureValues( ) { 
  
}

void MoveToPosition(float aPos) {
  
}

void MoveToAngle(float anAngle) {
  
}
 

 
