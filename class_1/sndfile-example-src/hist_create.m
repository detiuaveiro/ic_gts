%fileName = '../sndfile-example-bin/chMid.txt';
fileName = '../sndfile-example-bin/val.txt';
name = 'Plot for the left channel';
yLeg = 'Counter';
xLeg = 'Value';
T1 = readtable(fileName,'VariableNamingRule','preserve');

figure(1);
plot(T1{:,1},T1{:,2});
hold on;
xlabel(xLeg);
ylabel(yLeg);
title(name);
hold off;
