classdef CrossflowExperiment < handle 
    
    properties
        comPort = ""; % Com port for experiment equipment
        sPort; % Serial port 
        tcpPort;
        isDebug; 
        
        
        kSensorInit = uint8(15);
        kSensorSetRate = uint8(16);
        kPressureRequest = uint8(31);
        kAnemometerRequest = uint8(63);
        kAnemometerStreamRequest = uint8(64);
        kMotorMove = uint8(127);
        kResetMotorPos = uint8(255);
    end
    
    methods
        function obj = CrossflowExperiment(aPort) 
            % Create a crossflow experiment object to move motors and
            % sample data, the only argument needed is the COM port to
            % connect to the sensor and motor control box.
            % You can get a list of all available serial ports by using
            % 'seriallist' in the command window.
            % Example: ce = CrossflowExperiment("COM10") 
            if (strcmpi(aPort, "COM99"))
                obj.isDebug = 1; 
            else
                obj.isDebug = 0;
                obj.sPort = serial(aPort, 'BaudRate', 115200);
                obj.sPort.InputBufferSize = 1000000;
                fopen(obj.sPort);
                pause(1);
                ResetMotorPositions(obj);
                pause(1);
            end
        end
         
        
        function [calibrationData, cylinderData, wakeProfileData] = RunExperiment(obj, angleIncrement, vertIncrement)
            fprintf("\n\n");
            fprintf("Sensors should be open to atmospheric (lab) pressure - Unplug lines\n")
            userIn = input("Hit enter when ready to initialize sensors", 's');

            [ReferencePressures, ReferenceTemperatures] = InitializeSensors(obj);
            fprintf("Pressures (Pa): %6.2f, %6.2f, %6.2f, and Temperatures (C): %6.2f, %6.2f, %6.2f\n\n", ReferencePressures, ReferenceTemperatures)


            fprintf("\n\n");
            fprintf("Plug in Static (0), Stagnation (1), and Cylinder (2) pressure lines\n")
            userIn = input("Hit enter when ready to collect anemometer calibration data ", 's');
            
            calibrationData = [];
            sampleCount = 1;
            while (~strcmp(userIn, "s")) 
                fprintf("\nPressure and Anemometer Sample Run #%i\n", sampleCount);
                for i = 1:20
                    [pressures, anemometerReading] = GetAllSensorData(obj, 50);
                    calibrationData(end + 1,:) =  [sampleCount, pressures, anemometerReading];
                    fprintf("For Speed setting %i : Pressures (Pa): %6.4f, %6.4f, %6.4f, and anemometer (Volts): %6.4f\n", sampleCount, pressures, anemometerReading)
                    pause(0.5)
                end
                sampleCount = sampleCount + 1;
                fprintf("\n\n");
                userIn = input("Hit enter to collect more data, type 's' and hit enter to stop ", 's');
            end

            fprintf("\n\nLast step before running experiment: \n")
            fprintf("Align Cylinder to forward facing position and move anemometer to bottom position\n")
            ResetMotorPositions(obj);
            userIn = input("Hit enter when ready to run experiment ", 's');

            fprintf("\n\n")
            numSteps = round(180 / angleIncrement);
            fprintf("Sampling at %i increments\n", numSteps);
            for j = 0:numSteps 
                i = j * 180 / (numSteps);
                fprintf("Moving to angle = %f degrees (%i out of %i increments) \n", i, j, numSteps)
                MoveToCylinderAngle(obj, i);
                pause(2);
                [pressures, anemometerReading] = GetAllSensorData(obj, 50);
                cylinderData(j+1, :) = [i pressures];
            end
            
            fprintf("\nDone rotating cylinder\n\n")
            numSteps = round(240 / vertIncrement);
            for j = 0:numSteps 
                i = j * 240 / (numSteps);
                fprintf("Moving to vertial position = %6.2f mm \n", i)
                if (i > 0)
                    MoveToVerticalPosition(obj, i);
                end
                pause(1);
                [pressures, anemometerReading] = GetAllSensorData(obj, 50);
                wakeProfileData(j+1, :) = [i anemometerReading];
            end 
            fprintf("\nDone moving anemometer\nExperiment Complete, you'll find all data in returned arrays\n")
        end
        
        function [refPressures, refTemperatures] = InitializeSensors(obj) 
            % Initializes the MEMS pressure sensors and returns calibrated
            % pressure and temperature data from each sensor.  This data is
            % returned as two arrays, refPressures and refTemperatures.
            % Example: [refPressures, refTemperatures] = InitializeSensors(ce) 
            SendCommand(obj, [obj.kSensorInit]) 
            pause(1);
            resp = ReadResponse(obj);
            refPressures = [resp(1) resp(3) resp(5)];
            refTemperatures = [resp(2) resp(4) resp(6)];
        end
        
        function [anemometerReadings, timeBetweenReadings] = StreamAnemometerData(obj, streamTimePeriod) 
            % Tells the board to stream anemometer readings for a specified
            % amount of time. 
            % Example: [anemometerReadings, timeBetweenReadings] = 
            % StreamAnemometerData(ce, 5)  %% Streams data for 5 seconds
            % (1ms intervals)
            if (nargin < 2) 
                streamTimePeriod = 5;
            end
            tic;
            commandList = [obj.kAnemometerStreamRequest, uint8(streamTimePeriod)];
            SendCommand(obj, commandList); 
            responseData = ReadResponse(obj); 
            anemometerReadings = responseData;
            anemometerReadings = anemometerReadings(1:end-1);
            timeBetweenReadings = anemometerReadings(1:2:end);
            anemometerReadings = anemometerReadings(2:2:end) / 8192 * 3.3;;
        end
        
        function [pressures, anemometerReading] = GetAllSensorData(obj, averageOver) 
            % Gets sensor readings from all three pressure sensors and from
            % the hot wire anemometer.  The pressure readings are in
            % Pascals and returned in an array, the anemometer reading is
            % returned as a voltage between 0 Volts and 3.3 Volts.
            % If you specify a 2nd argument (averageOver), it will take
            % multiple samples and average  the pressure and anemometer
            % readings over that number of samples.
            % Example: [pressures, anemometerReading] = GetAllSensorData(ce, 50)
            if (nargin < 2) 
                averageOver = 1;
            end
            commandList = [obj.kPressureRequest, uint8(averageOver)];
            SendCommand(obj, commandList);
            responseData = ReadResponse(obj);
            pressures = responseData(1:3);
            anemometerReading = responseData(4) / 8192 * 3.3;
        end
        
        function pressures = GetPressureValues(obj, averageOver)
            % Gets sensor readings from all three pressure sensors.
            % The pressure readings are in Pascals and returned in an array.
            % If you specify a 2nd argument (averageOver), it will take
            % multiple samples and average the pressure readings over that 
            % number of samples.
            % Example: pressures = GetPressureValues(ce, 50)
            pressures = GetAllSensorData(obj, averageOver); 
        end
        
        function anemometerReading = GetAnemometerValue(obj, averageOver)
            % Gets sensor readings from the hot wire anemometer. 
            % The anemometer reading is
            % returned as a voltage between 0 Volts and 3.3 Volts.
            % If you specify a 2nd argument (averageOver), it will take
            % multiple samples and average the anemometer
            % readings over that number of samples.
            % Example: anemometerReading = GetAnemometerValue(ce, 50)
            [pressures, anemometerReading] = GetAllSensorData(obj, averageOver); 
        end
        
        function MoveToVerticalPosition(obj, targetPosition)  
            if (obj.isDebug == 1)
                fprintf("Moving to position of %f mm\n", targetPosition);
            end
            % Moves the vertical axis to the specified target Position (in mm).
            % Smallest vertical movement is 0.2mm.
            % Example: MoveToVerctialPosition(ce, 52.7)
            %fprintf("Moving to position of %f mm\n", targetPosition);
            commandList = [obj.kMotorMove, uint8(0), typecast(single(targetPosition), 'uint8')];
            SendCommand(obj, commandList)
            WaitForMoveResponse(obj); 
            pause(0.1);
        end
        
        function MoveToCylinderAngle(obj, targetAngle)  
            if (obj.isDebug == 1)
                fprintf("Moving to angle of %f degrees\n", targetAngle);
            end
            % Moves the cylinder to the specified target angle (in degrees).
            % Smallest angle movement is 1.8 degrees.
            % Example: MoveToCylinderAngle(ce, 97.2)
            %fprintf("Moving to angle of %f degrees\n", targetAngle);
            commandList = [obj.kMotorMove, uint8(1), typecast(single(targetAngle), 'uint8')];
            SendCommand(obj, commandList);
            WaitForMoveResponse(obj);
            pause(0.1);
        end
        
        function ResetMotorPositions(obj)
            % Resets motor positions to be zero - If you want to complete
            % more than one run without disconnecting or resetting.
            % Example: ResetMotorPositions(ce)
            SendCommand(obj, obj.kResetMotorPos);
            WaitForMoveResponse(obj);
            pause(0.25);
        end
        
        function SetSamplesPerSecond(obj, aSensorRate)  
            % Allows user to set the number of samples taken per second
            % for the LPS22HB sensors
            commandList = [obj.kSensorSetRate, uint8(aSensorRate) ];
            SendCommand(obj, commandList);
            sampleRateResponse = ReadResponse(obj);
            pause(0.25);
        end
        
        function SendCommand(obj, aCommand)
            if (obj.isDebug == 0) 
                %disp("Sending command: " + aCommand)
               flushinput(obj.sPort);
               %fprintf(1, "Sending command: ")
               %for i = 1:length(aCommand)
               %fprintf(1, "" + char(aCommand ))
               %fprintf(1, ", ") 
               %aCommand
               fwrite(obj.sPort, aCommand, 'uint8');
               %end
               %fprintf(1, "\n ") 
            end
        end
        
        
        function WaitForMoveResponse(obj) 
            if (obj.isDebug == 1)
                return;
            end
            % This function waits for a response from the C++ code on the
            % Teensy mc.  It blocks until it receives "Done Moving"
            serialTimer = tic;
            data = "";
            while(toc(serialTimer) < 25) % Enter the while loop for 5 seconds, you will want to change this to match the number of expected samples
                out = get(obj.sPort, 'BytesAvailable'); %get the number of bytes sitting in the Serial buffer
                if (out > 0) % if there is information waiting, go into this loop
                    data = data + fscanf(obj.sPort);
                    %fprintf(1, data + "\n")
                end
                if contains(data, newline) || contains(data, char(13))
                    if (contains(data, "Done Moving"))
                        %data;
                        return;
                    end
                end 
                pause(0.01);
            end
            warning("MATLAB never received a response signalling movement was completed, proceed with caution");
        end
        
        function response = ReadResponse(obj)
            if (obj.isDebug == 1)
                response = [1 2 4 8 16 32 64 128];
                return;
            end
            % This function waits for data to be returned from the C++
            % code on the Teensy mc.
            response = [];
            serialTimer = tic;
            data = "";
            while(toc(serialTimer) < 15) % Enter the while loop for 5 seconds, you will want to change this to match the number of expected samples
                out = get(obj.sPort, 'BytesAvailable'); %get the number of bytes sitting in the Serial buffer
                if (out > 0) % if there is information waiting, go into this loop
                    data = data + fscanf(obj.sPort);
                    %fprintf(1, data + "\n")
                else 
                    pause(0.01);
                end
                if contains(data, newline) || contains(data, char(13))
                    splitData = split(data,"//");
                    for i = 2:2:length(splitData)
                        response(i/2) = str2double(splitData(i));
                    end
                    %disp("Data rec: " + data)
                    return;
                end
            end
            warning("MATLAB never received a response signalling movement was completed, proceed with caution");
        end
        
        % Close the serial port, otherwise you'll have a hard time
        % connecting later
        function close(obj)
            fclose(obj.sPort);
        end 
    end
end

