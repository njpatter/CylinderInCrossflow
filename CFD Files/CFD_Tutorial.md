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
	* Double click on 'Setup - General'  
		* Select 
			* 'Pressure-Based'
			* 'Transient'
			* 'Absolute'
			* 'Planar' 
	* Expand 'Setup - Models'  
		* Double click on 'Viscous' 
			* Select 'Laminar'
			* Click 'OK' 
	* Double click on 'Setup - Boundary Conditions'
		* Set the following boundary conditions
			* Set cylinder_wall to type 'wall'
			* Set outlet to type 'pressure-outlet'
			* Set inlet to 'velocity-inlet' and click on 'Edit'
				* Change 'Velocity Specification Method' to 'Components' 
				* Set 'X-Velocity' to 28.5 m/s
				* Click 'Apply'
				* Click 'Close'
	* Double click on 'Setup - Reference Values'
		* Set 'Compute from' to 'inlet'
		* Set 'Area' to 1 m^2
	* Double click on 'Solution - Report Definitions'
		* Click 'New' - 'Force Report' - 'Drag'
			* Change 'Name' to 'timeavg-drag'
			* Select 'Drag Force'
			* Select 'cylinder_wall'
			* Set 'Average Over' to 32
			* Click 'OK'
		* Click 'New' - 'Force Report' - 'Lift'
			* Change 'Name' to 'lift-force
			* Select 'Lift Force'
			* Select 'cylinder_wall'
			* Set 'Average Over' to 1
			* Click 'OK'
		* Click 'Close'
	* Expand 'Solution - Monitors'
		* Double click on 'Residual'
			* Set 'Absolute Criteria' to 0.0001 for 'continuity', 'x-velocity', and 'y-velocity'
			* Click OK
	* Double click on 'Solution - Initialization'
		* Select 'Standard Initialization'
		* Set 'Compute from' to 'inlet'
		* Click 'Initialize'
	* Expand 'Solution - Calculation Activities'
		* Double click on 'Solution Animations'
			* Click 'New Object' then 'Contours'
				* Set 'Contour Name' to 'vorticity-contours'
				* Uncheck 'Auto Range'
				* Uncheck 'Clip to Range'
				* Set 'Contours of' to 'Velocity...' **'Vorticity Magnitude'**
				* Set Min to 0
				* Set Max to 1500
				* Click 'Save/Display'
				* Click 'Close'
			* Click on 'vorticity-contours' to highlight it
			* Set 'Name' to 'vorticity-animation'
			* Set 'Record after every' to 1 'time-step'
			* Use your mouse to zoom in on the cylinder at the center of the domain.  Set up a view so the cylinder is at the left hand side of the viewing window and there is space to be able to see vorticies form behind the cylinder.
			* Click 'Use Active'
			* Click 'OK'
	* Double click on 'Solution - Run Calculation'
		* Set 'Number of Time Steps' to 750
		* Set 'Time Step Size' to 0.0002
		* Click 'Calculate'
	
## Simulation Intermission
Enjoy a coffee or other beverage while you watch Fluent iteratively determine a numerical solution at each time step and then advance the simulation through time.
		
		
## Saving Data, Finding Data, and Solution Playback	
After the simulation has completed...
* Expand 'Results - Animations'
	* Double click on 'Playback'
		* Click the play button to watch the formation and procession of the Karman Vortex Street 
	* Change 'Write/Record Format' to 'Video'
	* Click 'Write'
* Expand 'Results - Plots'
	* Double click on 'XY Plot'
		* Set 'Name' to 'cylinderpressure'
		* Check 'Write to File'
		* Set 'Y Axis Function' to 'Pressure...' 'Static Pressure'
		* Set 'X Axis Function' to 'Curve Length'
		* Highlight 'cylinder_wall'
		* Click 'Write' and save to known location
		* Click Close 
* Using File Explorer, Open the 'CylinderInCrossflow_files' directory
	* Open sub-directory 'dp2/FFF/Fluent' 
	* Use data in 'lift-force-rfile.out' to visualize lift-force oscillations
	* Use data in 'timeavg-drag-rfile.out' to obtain time-averaged drag values
	* Use cylinderpressure data to plot static pressure and pressure coefficients on the cylinder surface
		
		
		
		
