# Cylinder In Crossflow - Ansys Tutorial

## Introduction
The purpose of this tutorial is to simulate a cylinder in a crossflow. This simulation allows for numerical and visual examination of the flow field, stresses, and resulting forces that are experimentally determined in the accompanying physical lab activity.

## Ansys Workbench & Fluent Tutorial

* Download CFD Files
	* Extract and move Workbench Project file (CylinderInCrossflow.wbpj) and accompanying folder (CylinderInCrossflow_files) to known location
* Open Ansys Workbench
	* File - Open 
		* Locate and open CylinderInCrossflow.wbpj file
	* Double click 'Setup' 
		* Under Options, select 'Double Precision'
		* Click 'Start' to launch Fluent
	* Click on 'Setup - General'  
		* Select 
			* 'Pressure-Based'
			* 'Transient'
			* 'Absolute'
			* 'Planar' 
	* Expand 'Setup - Models'  
		* Double click on 'Viscous' 
			* Select 'Laminar'
			* Click 'OK'
	* Expand 'Setup - Materials'  
		* Select air and click 'Create/Edit'
			* To obtain a Re = 68000, change Density to 1 kg/m^3 and Viscosity to 0.0000147
			* Click 'Change/Create'
			* Click 'Close'
	* Double click on 'Setup - Boundary Conditions'
		* Set the following boundary conditions
			* Set cylinder_wall to type 'wall'
			* Set outlet to type 'pressure-outlet'
			* Set inlet to 'velocity-inlet' and click on 'Edit'
				* Change 'Velocity Specification Method' to 'Components' 
				* Set 'X-Velocity' to 1 m/s
				* Click 'Apply'
				* Click 'Close'
			* 
		
		
		
		