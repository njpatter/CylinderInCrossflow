% Clear out old commands and serial connections
clc; clear all; close all; instrreset;

% This is how you establish a connection to the Teensy
ce = CrossflowExperiment("COM10");

% Initialize sensors
ce.InitializeSensors();

% Get pressure and anemometer values averaged over 50 values
ce.GetAllSensorData(50);