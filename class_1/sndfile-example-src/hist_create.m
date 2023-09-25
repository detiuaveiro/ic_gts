%fileName = '../sndfile-example-bin/chMid.txt';
fileName = '../sndfile-example-bin/chSide.txt';
T1 = readtable(fileName,'VariableNamingRule','preserve');

figure(1);
%plot(T1{:,1},T1{:,2})
histogram(T1{:,1});
