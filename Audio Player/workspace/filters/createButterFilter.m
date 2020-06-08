%%
% Designs a butterworth filter
% Parameters: 
% order: filter order
% fs: sampling frequency [Hz]
% fc: cutoff frequency [Hz], for bandpass and bandstop filters a vector [fcmin
% fcmax] must be passed
% ftype: one of the strings "lowpass","highpass","bandpass","bandstop"
% Returns the numerator and denominator coefficients of a digital filter
function [b,a]=createButterFilter(order,fs,fc,ftype)
Wn=fc/(fs/2);           % normalized cutoff frequency
if(strcmp(ftype, "lowpass") == 1)
        [b,a] = butter(order,Wn,'low');
elseif(strcmp(ftype, "highpass") == 1)
       [b,a] = butter(order,Wn,'high');
elseif(strcmp(ftype, "bandpass") == 1)
        if length(Wn) ~= 2
            error('Pass fc as a vector with two cutoff frequencies to design a bandpass filter!')
        end
        [b,a] = butter(order,Wn);       
elseif(strcmp(ftype, "bandstop") == 1)
        if length(Wn) ~= 2
            error('Pass fc as a vector with two cutoff frequencies to design a bandstop filter!')
        end
        [b,a] = butter(order,Wn,'stop');       
else
        error('Unknown filter type! Type <help createButterFilter> in the command window for help. ')
end
%fvtool(b,a)
[h,f] = freqz(b,a,128,fs);
figure
semilogx(f,20*log10(abs(h)))
xlabel('frequency in Hz')
ylabel('Magnitude in dB')
title("Butterworth order="+num2str(order)+" fs="+ num2str(fs)+" fc="+num2str(fc)+" "+ftype)
grid