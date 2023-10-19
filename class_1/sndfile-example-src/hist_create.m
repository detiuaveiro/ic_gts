%fileName = '../sndfile-example-bin/chMid.txt';
fileName = '../sndfile-example-bin/val.txt';
name = 'Plot for the left channel';
yLeg = 'Counter';
xLeg = 'Value';
T1 = readtable(fileName,'VariableNamingRule','preserve');
values = T1{:,1};
counts = T1{:,2};

figure(1);
bar(values, counts);
%plot(values, counts);
hold on;
xlabel(xLeg);
ylabel(yLeg);
title(name);
hold off;
