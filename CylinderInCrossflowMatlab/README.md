# Crossflow Experiment MATLAB Class
The following is all of the class constructor and methods that you will need to complete the cylinder in a crossflow experiment.  Make sure to initialize the sensors before you try to use them!



## CrossflowExperiment
  Create a crossflow experiment object to move motors and
  sample data, the only argument needed is the COM port to
  connect to the sensor and motor control box.
  You can get a list of all available serial ports by using
  'seriallist' in the command window.

  Example: ce = CrossflowExperiment("COM10")


## CrossflowExperiment/InitializeSensors
  Initializes the MEMS pressure sensors and returns calibrated
  pressure and temperature data from each sensor.  This data is
  returned as two arrays, refPressures and refTemperatures.

  Example: [refPressures, refTemperatures] = InitializeSensors(ce)


## CrossflowExperiment/GetAllSensorData
  Gets sensor readings from all three pressure sensors and from
  the hot wire anemometer.  The pressure readings are in
  Pascals and returned in an array, the anemometer reading is
  returned as a voltage between 0 Volts and 3.3 Volts.
  If you specify a 2nd argument (averageOver), it will take
  multiple samples and average  the pressure and anemometer
  readings over that number of samples.

  Example: [pressures, anemometerReading] = GetAllSensorData(ce, 50)


## CrossflowExperiment/GetAnemometerValue
  Gets sensor readings from the hot wire anemometer. 
  The anemometer reading is
  returned as a voltage between 0 Volts and 3.3 Volts.
  If you specify a 2nd argument (averageOver), it will take
  multiple samples and average the anemometer
  readings over that number of samples.

  Example: anemometerReading = GetAnemometerValue(ce, 50)

## CrossflowExperiment/GetPressureValues
  Gets sensor readings from all three pressure sensors.
  The pressure readings are in Pascals and returned in an array.
  If you specify a 2nd argument (averageOver), it will take
  multiple samples and average the pressure readings over that 
  number of samples.

  Example: pressures = GetPressureValues(ce, 50)


## CrossflowExperiment/MoveToCylinderAngle
  Moves the cylinder to the specified target angle (in degrees).
  Smallest angle movement is 1.8 degrees.

  Example: MoveToVertialPosition(ce, 97.2)


## CrossflowExperiment/MoveToVerticalPosition
  Moves the vertical axis to the specified target Position (in mm).
  Smallest vertical movement is 0.2mm.

  Example: MoveToVerticalPosition(ce, 52.7)


## CrossflowExperiment/StreamAnemometerData
  Tells the board to stream anemometer readings for a 
  specified amount of time. Samples every 1ms.

  Example: [timeBetweenReadings, anemometerReadings] = StreamAnemometerData(ce, 5)   
