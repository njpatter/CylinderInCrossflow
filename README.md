# CylinderInCrossflow
Cylinder in a Crossflow Experiment 

## Introduction
This repository contains the code and instructions required for running each of the experiment versions and CFD simulation described in the ASME IMECE 2021 conference paper (citation and link when they are ready).

## CFD Tutorial
CFD_Tutorial markdown file with step-by-step instructions can be found in the 'CFD Files' directory.

## LPS22HB Library
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

## Potential Flow Problems
Problem 1: Use potential flow theory to derive an expression for pressure on the surface of a cylinder in a crossflow.  Create a plot of the pressure percent difference, relative to the pressure at the front stagnation point, as a function of the angle (a) from horizontal from 0 to 90 degrees. 

Problem 2: You built a 20ft long, 10ft radius Quonset hut-based cabin on flat ground and it weights 1000lbs. Your antagonistic neighbor does not like the economical and light-weight design, so they drilled a hole in the side of the cabin prior to a windstorm.  If the wind is blowing perpendicular to the axis of your cabin and parallel to the ground, at what wind velocity will your house start to lift off the ground? You do not know the angle (a), from horizontal, that your neighbor drilled the hole so create a plot relating angle and expected take-off wind velocity. 


## But what if I don't have a wind tunnel?
Good news! I put together a simulator version of the physical lab. It can be found in this repository:
https://github.com/njpatter/FluidMechanicsExperiments

Additionally, the same repo has web-based simulators of a few other fluids labs:
https://njpatter.github.io/FluidMechanicsExperiments/ 

## Questions?
email me: patterson (at) msoe.edu
