close all; clc;clear all
%% check the butterworth filter
% enter the name of the sound you used in C++ here
sndname='hey';
[xk,fs]= audioread(['..\..\Sounds\' sndname '.wav']);
[ck]= audioread(['..\files\' sndname '_butter_filtered.wav']);

% create and apply the MATLAB butterworth filter
[b,a] = createButterFilters(fs,0);  % filter coefficients
mk=filter(b,a,xk);                  % filter the signal

% plot to compare the filtered signals 
tck=(0:length(ck)-1)/fs; % time vector for the C++ filtered signal
tmk=(0:length(mk)-1)/fs; % time vector for the MATLAB filtered signal

% compare left channels
subplot(2,1,1)
plot(tck,ck(:,1),tmk,mk(:,1))
legend('ck - filtered by C++','mk - filtered by MATLAB')
xlabel('t/s')
title('Left channel')
% compare right channels
subplot(2,1,2)
plot(tck,ck(:,2),tmk,mk(:,2))
legend('ck - filtered by C++','y1k - filtered by MATLAB')
xlabel('t/s')
title('Right channel')

% compare signals by playing
disp({'(1) original','(2) C++ filtered' '(3) MATLAB filtered'}')
plObj=audioplayer([xk;ck;mk],fs);
playblocking(plObj);
