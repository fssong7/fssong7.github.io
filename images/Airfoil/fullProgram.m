function fullProgram()
alphaArray = zeros(1,11);
elevatorArray = zeros(1,11);
ClArray = zeros(1,11);
CdArray = zeros(1,11);
CmArray = zeros(1,11);

for i=0:10
    preProcessing(i,i);
    Processing();
    [alpha,elevator,Cl,Cd,Cm] = postProcessing();
    alphaArray(i+1) = alpha;
    elevatorArray(i+1) = elevator;
    ClArray(i+1) = Cl;
    CdArray(i+1) = Cd;
    CmArray(i+1) = Cm;
end


figure
plot3(alphaArray,ClArray,elevatorArray,alphaArray,CdArray,elevatorArray,alphaArray,CmArray,elevatorArray)
title('Angle of Attack vs. Coefficient vs. ElevatorAngle')
xlabel('Alpha(degrees)')
ylabel('Coefficient')
zlabel('Elevator(degrees)')
legend('Coefficient of Lift','Coefficient of Drag','Coefficent of Moment')
