function [b,a,order,fc,ftype] = createButterFilters(fs,bplot)
%%
% Designs a butterworth filter
% this function is designed to make sure that the same filter is 
% used in writing the filter file and checking the sounds
%
% Parameters: 
% fs: sampling frequency [Hz]
% bplot: 1 ... plot the amplitude response, 0 ... do not plot
%
% Returns 
% b ... the numerator 
% a ... the denominator coefficients of a digital filter
% order ... chosen order
% fc ... chosen cuttoff frequency
% type ... chosen filter type
%
%% choose analogue butterworth filter parameters
order=6;            % filter order
fc=2000;            % cutoff frequency [Hz],  
                    % for bandpass and bandstop filters a vector [fcmin fcmax] must be passed
ftype="lowpass";    % filter type use "lowpass","highpass","bandpass","bandstop"

% filter design
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

% plot the amplitude response
if bplot == 1
    [h,f] = freqz(b,a,128,fs);
    figure
    semilogx(f,20*log10(abs(h)))
    xlabel('frequency in Hz')
    ylabel('Magnitude in dB')
    title("Butterworth order="+num2str(order)+" fs="+ num2str(fs)+" fc="+num2str(fc)+" "+ftype)
    grid
end
