channels = [2, 1];
initialSize = [5.18, 1.1]; % in MB
encodedSize = [3.3, 0.703]; % in MB
timeFixed = [2.5, 0.22]; % in seconds
oldBitrate = [1411, 705]; % in kbps
newBitrate = [300, 300]; % in kbps

% Plotting the data using grouped bar graphs
figure;

subplot(2, 2, 1);
bar(channels, initialSize);
xlabel('Channels');
ylabel('Initial Size (MB)');
title('Initial Size');

subplot(2, 2, 2);
bar(channels, encodedSize);
xlabel('Channels');
ylabel('Encoded Size (MB)');
title('Encoded Size');

subplot(2, 2, 3);
bar(channels, timeFixed);
xlabel('Channels');
ylabel('Time (seconds)');
title('Time Encoding/Decoding (Fixed Parameters)');

subplot(2, 2, 4);
bar(channels, [oldBitrate; newBitrate]');
legend('Old Bitrate', 'New Bitrate', 'Location', 'northwest');
xlabel('Channels');
ylabel('Bitrate (kbps)');
title('Bitrate Comparison');

xticklabels({'2', '1'}); % Setting x-axis labels for channels
