
% Clear out old commands and serial connections
clc; clear all; close all; instrreset;

% This is how you establish a connection to the Teensy
ce = CrossflowExperiment("COM10");
 
% Command for running an automated experiment given angle and vertical
% increments
angleInc = 3.6; % degrees
vertInc = 2.4; % mm
[calibrationData, cylinderData, wakeProfileData] = ce.RunExperiment(angleInc, vertInc);