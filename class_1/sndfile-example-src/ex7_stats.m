%% Settings:
% Block Size: 1024
%  - Number of Channels: 1
%  - Sample Rate: 44100
%  - Quantization Levels: 8
%  - Dct Frac: 0.2
fileNames = ['sample.wav', 'sample03.wav', 'sample06.wav', 'sample01.wav'];
fileSizes = [2.1, 3.5, 4.2, 5.2];    % in mb
timeEncoding = [88.518, 136.423, 168.102, 214.068];  % in ms
timeDecoding = [116.541, 170.022, 205.185, 246.608];  % in ms
encodedFileSize = [529.4, 882.7, 1100, 1300];   % in kbs
decodedFileSize = [1.1, 1.8, 2.1, 2.6];   % in mb

figure(1);
subplot(2, 1, 1);
plot(fileSizes, timeEncoding);
hold on
title('Time Encoding based on File Size');
xlabel('File Size (Mb)');
ylabel('Encoding time (Ms)');
hold off

subplot(2, 1, 2);
plot(fileSizes, timeDecoding);
hold on
title('Time Decoding based on File Size');
xlabel('File Size (Mb)');
ylabel('Encoding time (Ms)');
hold off

figure(2);
subplot(2, 1, 1);
plot(fileSizes, encodedFileSize);
hold on
title('Encoded Size based on Original File Size');
xlabel('Original File Size (Mb)');
ylabel('Encoded File Size (Kbs)');
hold off

subplot(2, 1, 2);
plot(fileSizes, decodedFileSize);
hold on
title('Decoded File Size based on Original File Size');
xlabel('Original File Size (Mb)');
ylabel('Decoded File Size (Mb)');
hold off

avgEncodedRate = encodedFileSize ./ (fileSizes.*1000);
avgEncodedRate = mean(-(avgEncodedRate - 1));
avgDecodedRate = decodedFileSize ./ fileSizes;
avgDecodedRate = mean(-(avgDecodedRate - 1));
fprintf('The reduction in the encoded file size, compared to the original is %.f %%\n', avgEncodedRate*100);
fprintf('The reduction in the resulting audio file size, compared to the original is %.f %%\n', avgDecodedRate*100);