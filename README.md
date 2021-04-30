# CylinderInCrossflow
Cylinder in a Crossflow Experiment 

## Introduction
This repository contains the code required for running each of the version described in the ASME IMECE 2021 conference paper (citation and link when they are ready).

## Library
Each version requires the LPS22HB library located in the LPS22HB folder. There is a more comprehensive version of this library available through multiple sources, however this version was created to work with the 3 LPS22HB setup used in the Cylinder in a Crossflow experiment.

## FromScratch
This is a barebones script in which students will need to program the majority of the experimental setup and data collection.  Write the program and then upload the program to the Teensy 3.2.

## Cpp
This contains the code to run both the Vortex Shedding lab portion and the Total/Pressure/Viscous Drag lab portion.  Upload the code to the Teensy 3.2 and you are ready to go.

## Matlab
This contains two possible approaches and challenges for students.  

The first is the 'Simplified Matlab' approach.  This requires that the included C++ code be compiled and uploaded to the Teensy proir to students using the included Matlab class.  Students will need to use the included CrossflowExperiment class and program all movement and data sampling.

The second is the 'Automated' approach.  This is exactly what it sounds like.  Students can perform the entire experiment using a few lines of Matlab code and are provided back arrays containing all collected data.

## Schematic
Here's a hastily put together schematic for the experimental setup:
![image](https://user-images.githubusercontent.com/34224135/116077465-2355ea80-a65b-11eb-8df5-0ca5c8ba87d2.png)

## Questions?
email me: patterson (at) msoe.edu
