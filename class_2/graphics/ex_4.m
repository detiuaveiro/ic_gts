% Data
channels = [2, 1];
initialSize = [5.18, 1.1]; % in MB
encodedSize = [4.5, 0.801]; % in MB
timeAuto = [310, 19.79]; % in seconds
timeFixed = [0.18, 0.18]; % in seconds

% Plotting
figure;

subplot(2, 2, 1);
bar(channels, initialSize);
xlabel('Channels');
ylabel('Size (MB)');
title('Initial Size');

subplot(2, 2, 2);
bar(channels, encodedSize);
xlabel('Channels');
ylabel('Size (MB)');
title('Encoded Size');

subplot(2, 2, 3);
bar(channels, timeAuto);
xlabel('Channels');
ylabel('Time (seconds)');
title('Time Encoding/Decoding (Automatic)');

subplot(2, 2, 4);
bar(channels, timeFixed);
xlabel('Channels');
ylabel('Time (seconds)');
title('Time Encoding/Decoding (Fixed Parameters)');
