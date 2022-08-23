
% Clear out old commands and serial connections
clc; clear all; close all; instrreset;

% This is how you establish a connection to the sensor box or simulator
ce = CrossflowExperiment("COM7");
 
disp("Starting samples")
% Command for streaming Anemometer data from the sensor box or simulator
[anemometerData, msBetweenReadings] = StreamAnemometerData(ce, 3) ;

close all;