#include <Arduino.h>
#include <SPI.h> 
#include <Adafruit_ADS1X15.h>



/* 
C++ - 
  Wait for User Input on Vortex vs Drag Experiment
  Init appropriate sensors
  Move vertical/cylinder as needed
  Sample pressure sensors and anemometer
  Print data to serial monitor or other program listening
  

*/  

// ADS1115 product page: https://www.adafruit.com/product/1085
// ADS1115 library repo: https://github.com/nipoutch/Adafruit_ADS1115
Adafruit_ADS1115 ads; 
// Pressure sensor datasheet: https://www.allsensors.com/datasheets/DS-0368_Rev_A.PDF
const int numPressureSensors = 4;

float sensorInitVoltages[numPressureSensors] = {0.0, 0.0, 0.0, 0.0};
float sensorVoltages[numPressureSensors] = {0.0, 0.0, 0.0, 0.0};
float pressureValues[numPressureSensors] = {0.0, 0.0, 0.0, 0.0};
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
bool isInitialized = false;

int millisDelayAfterMoves = 3000;
int stepDelayMillis = 20; // Could be lowered to get faster movement
long tempSensorReading = 0;

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

  //ads.setGain(GAIN_ONE);
  ads.begin();
  delay(1000);
  
  Serial.begin(9600);
  analogReadResolution(13);
  while (!Serial) {;} 
}

// Main loop - only use is for sampling and sending anemometer
// data for vortex shedding experiment 
/*
 * UpdateAndAveragePressures((byte)inByte); //sampleAverageCount);
        PrintPressureValues(); 
 * 
 */
void loop() {  
  HandleMenu(); 
}

void HandleMenu() {
  Serial.println("\n\nPress 1 and press enter to stream anemometer values");
  Serial.println("Press 2 and press enter to run drag experiment");
  Serial.println("Press 3 to set pressure sensor update rate (75 / sec is default)");
  Serial.println("Press 4 to set millis day after each motor move");
  Serial.println("Press 5 to set number of sensor readings to take and average over per reported value");
  Serial.println("Press 6 to get current pressure and anemometer values");
  Serial.println("Press 7 to re-initialize sensors");
  Serial.println("Press 8 to print out initial pressure and temperature readings");
  Serial.println("Press 9 to print out pressure sensor voltages (avg 100 samples)");
  Serial.println("");
  
  int menuChoice = GetUserIntInput();
  Serial.flush();
  if (menuChoice == 1) { 
    Serial.println("Choose integer number of seconds for anemometer stream (1-10 seconds) and press enter");  
    menuChoice = GetUserIntInput();
    if (menuChoice > 0.99) { 
      Serial.println(menuChoice);
      StreamAnemometerData(menuChoice);
    }
  } 
  else if (menuChoice == 2) {
    RunDragExperiment();
  }
  else if (menuChoice == 3) {
    Serial.println("Choose an update rate (available rates are 1-100 samples per second) and hit enter"); 
    menuChoice = GetUserIntInput();
    if (!isInitialized) {
      InitSensors();
    }
    SetSampleRate(menuChoice);
  }
  else if (menuChoice == 4) {
    Serial.println("Choose a movement delay in milliseconds - The time paused following a move before taking a pressure/anemometer reading");
    millisDelayAfterMoves = GetUserIntInput();
    Serial.print("Delay set to ") ;
    Serial.print(millisDelayAfterMoves);
    Serial.println(" milliseconds");
  }
  else if (menuChoice == 5) {
    Serial.println("Set number of sensor values to average over (1-100 suggested)");
    sampleCountForAvg = GetUserIntInput();
    Serial.print("Sensor average count set to: ");
    Serial.print(sampleCountForAvg);
    Serial.println("");
  }
  else if (menuChoice == 6) {
    Serial.println("Ready to read pressures, set number of sensor values to average over (1-100 suggested) and hit enter:");
    menuChoice = GetUserIntInput();
    if (!isInitialized) {
      InitSensors();
    }
    Serial.print("With the current update rate of "); 
    Serial.print(sampleFreq);
    Serial.print(", the readings are expected to take ");
    Serial.print((float)menuChoice / (float)sampleFreq);
    Serial.println(" seconds");
    
    UpdateAndAveragePressures(menuChoice);
    PrintPressureValues();
  }
  else if (menuChoice == 7) {
    InitSensors();
  } 
  else if (menuChoice == 9) {
    PrintPressureVoltageAvgs();
  } 
}

void RunDragExperiment() {
  int inputVal = 0;
  if (!isInitialized) {
    Serial.println("Type any key and press enter when ready to initialize sensors");
    Serial.println("(pressure lines should be unplugged so sensors will be centered at atmospheric pressure)");
    inputVal = GetUserIntInput();
    
    InitSensors();
  }
  else {
    Serial.println("Sensors already initialized, continuing to next section");
  }
  
  int tempInput = Serial.parseInt();
  Serial.println("\nThe following section prints out pressure and anemometer values to help with calibration,");
  Serial.println("press any key and enter when ready");
  Serial.println("Press any key and enter a second time to stop samples");
  inputVal = GetUserIntInput();
  while (Serial.available() < 1) {
    UpdateAndAveragePressures(50);
    PrintPressureValues();
  }
  while (Serial.read() > 0) {;}

  Serial.println("\nPosition cylinder and anemometer to starting positions");
  Serial.println("(Cylinder start is pressure tap facing into flow - stagnation point)");
  Serial.println("(Anemometer start is at bottom of vertical axis movement)");
  currentPos = 0.0;
  currentAngle = 0.0;
  inputVal = GetUserIntInput();

  Serial.println("Rotating cylinder to obtain surface pressure values");
  for (float a = 0.0; a <= maxAngle + 0.001; a +=angleIncrement) {
    MoveToAngle(a);
    Serial.print("Current (angle, position) = (");
    Serial.print(currentAngle);
    Serial.print(", ");
    Serial.print(currentPos);
    Serial.print(") with ");
    UpdateAndAveragePressures(sampleCountForAvg);
    PrintPressureValues();
  }

  Serial.println("Moving anemometer to obtain wake values");
  for (float v = 0.0; v <= maxPos + 0.001; v +=verticalIncrement) {
    MoveToPosition(v);
    Serial.print("Current (angle, position) = (");
    Serial.print(currentAngle);
    Serial.print(", ");
    Serial.print(currentPos);
    Serial.print(") with ");
    UpdateAndAveragePressures(sampleCountForAvg);
    PrintPressureValues();
  }

  Serial.println("\nExperiment complete!!!");
}

void StreamAnemometerData(int secondsToStream) {
  float anemometer = 0.0;
  long totalStreamMicros = (long)secondsToStream * 1000000;
  long stime = micros(); 
  float timeSincePrevReading = stime;
  long currTime = stime-1000;
  long prevTime = stime;
  // Following loop prints anemometer data and microseconds
  // between samples.  Matlab parses the data and returns
  // the two relevant arrays.
  Serial.println("The following line can be pasted into Matlab:  ");  
  Serial.print("microsAndAnemometerReadings = [");
  while (totalStreamMicros  > micros() - stime) {
    prevTime = currTime;
    currTime = micros();
    timeSincePrevReading = (float)(currTime - prevTime);
    anemometer = (float)analogRead(anemometerPin);
    Serial.print(timeSincePrevReading);
    Serial.print(" , ");
    Serial.print(anemometer);  
    Serial.print(" ; ");
    delayMicroseconds(935);
  }
  Serial.println("];"); 
}

// Function to print out pressure  and anemometer values.
// Called when request for pressure values OR anemometer values is sent
// from Matlab
void PrintPressureValues( ) { 
  Serial.print("Pressures for sensors and Anemometer reading:  "); 
  for (int i = 0; i < numPressureSensors; i++) {
    Serial.print("Sensor (");
    Serial.print(i);
    Serial.print(") ");
    Serial.print(pressureValues[i]);
    Serial.print(" Pa, ");
  } 
  Serial.print(anemometerValue);
  Serial.println(" out of 8192");
  
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
    for(int j = 0; j < numPressureSensors; j++) { 
      pressureValues[j] += GetPressureReading(j) / (float)avgCount;
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
    delay(stepDelayMillis);
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
    delay(stepDelayMillis);
  }
  currentAngle = aNewAngle;
}

void InitSensors() {
  Serial.println("Initializing sensors...");
  isInitialized = true; 
  for (int i = 0; i < numPressureSensors; i++) {
    tempSensorReading = ads.readADC_SingleEnded(i); 
    sensorInitVoltages[i] = ads.computeVolts(tempSensorReading);
    Serial.println(sensorInitVoltages[i]);
  }
  
  //tpSensors.Begin(); 
  PrintReferencePressures();
}

void PrintReferencePressures() {
  for(int i = 0; i < 3; i++) { 
    Serial.print("Sensor " );
    Serial.print(i);
    Serial.print(" Reference Pressure initialized to: " );
    Serial.print(0); //tpSensors.GetReferencePressure(i));
    Serial.print(" Pascals (Pa) at Temperature : " );
    Serial.print(0); //tpSensors.GetTemperature(i)); 
    Serial.println(" degrees C ");
  } 
}

float tempPressure = 0.0;
float GetPressureReading(int sensorIndex) {
  tempSensorReading = ads.readADC_SingleEnded(sensorIndex); 
  tempPressure = (ads.computeVolts(tempSensorReading) - sensorInitVoltages[sensorIndex]) / 2.0 * 1250.0;
  //tempPressure = float(tempSensorReading - sensorInitValues[sensorIndex]) / (32768.0) * 1250.0 * 2.5 / 2.0;
  Serial.print(tempSensorReading);
  Serial.print(" Converted to ");
  Serial.println(tempPressure);
  return (ads.computeVolts(tempSensorReading) - sensorInitVoltages[sensorIndex]) / 2.0 * 1250.0;
}

void PrintPressureVoltageAvgs() {
  float someReadings[4] = {0.0, 0.0, 0.0, 0.0};
  for (int repCount = 0; repCount < 100; repCount++) {
    for (int i = 0; i < 4; i++) {
      someReadings[i] = someReadings[i] + GetPressureSensorVoltage(i) / 100.0;
    }
  }
  Serial.print(someReadings[0]);
  Serial.print(", ");
  Serial.print(someReadings[1]);
  Serial.print(", ");
  Serial.print(someReadings[2]);
  Serial.print(", "); 
  Serial.println(someReadings[3]);
}

float GetPressureSensorVoltage(int sensorIndex) {
  return ads.computeVolts(ads.readADC_SingleEnded(sensorIndex));
}

void SetSampleRate(int samplesPerSecond) {
  sampleFreq = samplesPerSecond;
  /*tpSensors.SetSamplesPerSecond(samplesPerSecond); 
  tpSensors.writeSettings();
  Serial.print("Sample Rate Updated To: ");
  int aRate = 0;
  switch (tpSensors.GetSamplesPerSecond()) {
    case 16:
      aRate = 1;
      break;
    case 32:
      aRate = 10;
      break;
    case 48:
      aRate = 25;
      break;
    case 64:
      aRate = 50;
      break;
    case 80:
      aRate = 75;
      break;  
  }
  sampleFreq = aRate;*/
  Serial.print("Sample Rate Updated To: ");
  Serial.print(sampleFreq); 
  Serial.println(" updates per second");
}

int GetUserIntInput() {
  while (Serial.read() > 0) {
    // Do nothing if random input has been provided beyond menu selection
  }
  while (Serial.available() == 0) { 
    // Do nothing and check again for user input
  }
  int userVal = Serial.parseInt();
  return userVal;
}
 

 
