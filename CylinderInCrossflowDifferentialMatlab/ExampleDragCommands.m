% Clear out old commands and serial connections
clc; clear all; close all; instrreset;

% This is how you establish a connection to the Teensy
ce = CrossflowExperiment("COM7");

% Initialize sensors
%ce.InitializeSensors();

% Get pressure and anemometer values averaged over 50 values
[pressures anemometer] = ce.GetAllSensorData(50)


vel = sqrt(2 * abs(pressures(2))/1.2)

