density = 1.2;
pdyn = mean(cylinderData(:,3));
velocity = sqrt(2*pdyn/density);

calVel = sqrt(2*calibrationData(:,3) / density);
calAne = calibrationData(:,6);
calFit = fit(calAne, calVel, 'poly2');


wakeVel = feval(calFit, wakeProfileData(:,2))
%figure
%plot(wakeVel, wakeProfileData(:,1))

angleInc = 3.6;
cylDiameter = 0.0335;
pressureDrag = 0;
for i  = 1:length(cylinderData)
    areaMult = 1;
    if (i == 1 || i == length(cylinderData))
        areaMult = 1/2; % Multiplies front and back facing slices by 1/2
    end 
    % Add horizontal force due to pressure acting on this section of pipe
    pressureDrag = pressureDrag + cylinderData(i,4) * areaMult * pi * cylDiameter * (angleInc / 360) * cos((i - 1) * pi * angleInc / 180);
end
% Multiplies resulting force by 2 - top and bottom of cylinder
pressureDrag = 2 * pressureDrag; 
fprintf("Drag force from pressure: %f\n", pressureDrag) 

% Calculate coefficient of pressure on surface of cylinder
cp = (cylinderData(:, 4) ) ./ mean(cylinderData(:,3) );


% Calculate inlet velocity
inletVel = sqrt(2 * mean((cylinderData(:,3)))/ density);
% Use volume flow out and average inlet velocity to determine inlet area
areaIn = sum(wakeVel) * (vertInc / 1000) / inletVel;

% Calculate momentum flux into control volume
totalDrag = density * inletVel * inletVel * areaIn;
for i = 1:length(wakeVel(:))
    % Subtract away momentum flux out of each small slice at exit (in wake)
    totalDrag = totalDrag - vertInc / 1000 * density * wakeVelMod(i ) * wakeVelMod(i );
end
fprintf("For velocity inlet of %f m/s with an area inlet of %f m^2, the difference in momentum is %f N\n", inletVel, areaIn, totalDrag)

fprintf("This means that with a pressure drag of %f N and a total drag (momentum difference) of %f N, the viscous drag is equal to %f N\n", pressureDrag, totalDrag, totalDrag - pressureDrag)

% Plot Anemometer-Velocity relationship data
plot(calFit, calAne, calVel)
title("Anemometer-Velocity relationship")
xlabel("Anemometer Readings (Volts)")
ylabel("Inlet Velocities (m/s)")
figure
% Plot coefficient of pressure on surface of cylinder
plot(cylinderData(:,1),cp)
title("Coefficient of pressure")
xlabel("Angle (Degrees)")
ylabel("Cp")
figure
% Plot wake profile data
plot(wakeVel(:), wakeProfileData(:,1)) 
title("Wake Profile")
xlabel("Velocity (m/s)")
ylabel("Vertical Position (mm)")

