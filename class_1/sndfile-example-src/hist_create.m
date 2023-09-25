T1 = readtable('histogram.txt','VariableNamingRule','preserve');

figure
plot(T1{:,1},T1{:,2})
