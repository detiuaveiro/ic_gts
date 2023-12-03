fileName = '../bin/decoded_values_distribution.txt';
T1 = readtable(fileName, 'VariableNamingRule', 'preserve');
header = T1.Properties.VariableNames;
name = 'Plot for the Error channel';
yLeg = splitString{2};
xLeg = 'Mono count';
values = T1{:, 1};

figure;
histogram(values);
xlabel(xLeg);
ylabel(yLeg);
title(name);
