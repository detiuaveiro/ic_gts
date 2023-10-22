fileName = '../sndfile-example-bin/val.txt';
T1 = readtable(fileName,'VariableNamingRule','preserve');
header = T1.Properties.VariableNames;
splitString = strsplit(header{2}, ' ');
name = ['Plot for the ', splitString{1}, ' channel'];
yLeg = splitString{2};
xLeg = header{1};
values = T1{:,1};
counts = T1{:,2};

figure(1);
bar(values, counts);
xlabel(xLeg);
ylabel(yLeg);
title(name);
