density = 1.18;
calVel = sqrt(2*calibrationData(:, 3)/density);

calFit = fit(calibrationData(:,6),calVel, "poly2")
plot(calFit, calibrationData(:,6),calVel)

wakeVel = feval(calFit, wakeProfileData(:,2))
figure
plot(wakeVel, wakeProfileData(:,1))
xlabel("Velocity (m/s)");
ylabel("Vertical Pos (mm)");

avgVel = sqrt(2*mean(cylinderData(:,3))/density);
cp = cylinderData(:,4)/(0.5*density*avgVel^2);
figure
plot(cylinderData(:,1),cp )
xlabel("Angle (deg)");
ylabel("Cp");

diameter = 0.0335;
dA = 3.6/360*diameter*pi;
Fx = cylinderData(:,4).*dA.*cosd(cylinderData(:,1));
Fx(1) = 0.5*Fx(1);
Fx(end) = 0.5*Fx(end);
pressureDrag = sum(Fx) * 2

dAout = 0.0024;
outMass = density*sum(dAout * wakeVel);
inletArea = outMass/(density*avgVel);

inletMom = density*avgVel^2*inletArea;
outletMom = density*dAout*sum(wakeVel.^2);
totalDrag = outletMom - inletMom
