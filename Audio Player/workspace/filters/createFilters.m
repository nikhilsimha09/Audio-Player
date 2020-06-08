close all; clc;clear all
[xk,fs]= audioread('..\..\Sounds\hey.wav');
play=0;

%% Butterworth filter design
[b, a, N, fc, type] = createButterFilters(fs,1); % filter coefficients
[yk]=filter(b,a,xk);
% write the filter coefficients
filename=writeFilterDataFileHeader("..\files\"+num2str(fc)+"Hz_",type,N,"cutoff frequency fc="+num2str(fc));
writeFilterData(filename,fs,b,a);

if play==1
    plObj=audioplayer([xk;yk],fs);
    playblocking(plObj);
end

% plot magnitude and spectrum
hold all
Yf=fft(yk);
Xf=fft(xk);
f=0:fs/length(xk):fs/2;
semilogx(f,20*log10(abs(Xf(1:length(f)))),f,20*log10(abs(Yf(1:length(f)))))
legend('filter magnitude','original','low pass filtered')
xlim([min(f) max(f)])

% designs for other sampling frequencies
fs1=16000;
[b,a] = createButterFilters(fs1,0); % filter coefficients
writeFilterData(filename,fs1,b,a);  % write the filter coefficients
fs1=11025;
[b,a] = createButterFilters(fs1,0); % filter coefficients
writeFilterData(filename,fs1,b,a);  % write the filter coefficients
fs1=22050;
[b,a] = createButterFilters(fs1,0); % filter coefficients
writeFilterData(filename,fs1,b,a);  % write the filter coefficients
fs1=48000;
[b,a] = createButterFilters(fs1,0); % filter coefficients
writeFilterData(filename,fs1,b,a);  % write the filter coefficients

%% Filters with a free frequency-magnitude characteristics
%% treble boost (shelving filter)
figure
[b,a,order,flin] = createYulewalkFilters(fs,1); 
% write the filter coefficients
filename=writeFilterDataFileHeader("..\files\yulewalk_","treble_boost",order,"boosting starts at < "+ num2str(flin) +" and goes upto the half sampling frequency");
writeFilterData(filename,fs,b,a);

% Test filter
[yk1]=filter(b,a,xk);

% play
if play==1
    plObj=audioplayer([xk;zeros(round(fs/4),1);yk1],fs);
    playblocking(plObj);
end

% spectrum
Yf=fft(yk1);                % filtered signal
Xf=fft(xk);                 % original signal
ffft=0:fs/length(xk):fs/2;    % appropriate frequency vector
% plots
hold all
semilogx(ffft,20*log10(abs(Yf(1:length(ffft)))),ffft,20*log10(abs(Xf(1:length(ffft)))))
xlabel('frequency in Hz')
ylabel('Magnitude in dB')
legend('Yule-Walker filter magnitude','ideal filter magnitude','trebel boosted','original')
grid

% designs for other sampling frequencies
fs1=16000;
[b,a] = createYulewalkFilters(fs1,0); % filter coefficients
writeFilterData(filename,fs1,b,a);  % write the filter coefficients
fs1=11025;
[b,a] = createYulewalkFilters(fs1,0); % filter coefficients
writeFilterData(filename,fs1,b,a);  % write the filter coefficients
fs1=22050;
[b,a] = createYulewalkFilters(fs1,0); % filter coefficients
writeFilterData(filename,fs1,b,a);  % write the filter coefficients
fs1=48000;
[b,a] = createYulewalkFilters(fs1,0); % filter coefficients
writeFilterData(filename,fs1,b,a);  % write the filter coefficients

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% delay effects
%% basic delay
tdelay=250e-3;      % delay time in ms
gFF=0.5;            % feedforward gain
[b,a]=createBasicDelayFilters(gFF,0,tdelay,fs);
[yk1]=filter(b,a,xk);

% playing
if play==1
    plObj=audioplayer([xk;zeros(round(fs/4),1);yk1/max(abs(yk1))],fs);
    playblocking(plObj);
end

%% basic delay with feedback
tdelay=50e-3;      % delay time in ms
gFF=0.5;           % feedforward gain
gFB=0.5;           % feedback gain
[b,a]=createBasicDelayFilters(gFF,gFB,tdelay,fs);
[yk1]=filter(b,a,xk);

% playing
if play==1
    plObj=audioplayer([xk;zeros(round(fs/4),1);yk1/max(abs(yk1))],fs);
    playblocking(plObj);
end

%% delay filters of very high order
tdelay=500e-3;
gFB=0.75;
gFF=0.5;
createBasicDelayFilters(gFF,gFB,tdelay,48000);
createBasicDelayFilters(gFF,gFB,tdelay,44100);
createBasicDelayFilters(gFF,gFB,tdelay,22050);
createBasicDelayFilters(gFF,gFB,tdelay,16000);
createBasicDelayFilters(gFF,gFB,tdelay,11025);

