function [b,a,order,flin] = createYulewalkFilters(fs,bplot)
%% Filters with a free frequency-magnitude characteristics
%% example: treble boost (shelving filter)

% parameters
order = 6;      % yulewalk filter order
% boosting starts at > flin an goes upto the half sampling frequency
flin=1000;

% 1. model an ideal filter amplitude response
f1=[flin+fs/128:fs/16:fs/2 fs/2];
% maximum raise shall be 12 dB (at the highest signal frequency)
raise=10^(12/20);               % linear raise
m=1+raise/(max(f1)-min(f1))*f1; % calculate linear equation
f = [0 flin f1];                % frequency vector
m = [1 1 m];                    % gain 1 until flin 
% 2. yulewalk filter design (according to the Yule–Walker equations, named for
% Udny Yule and Gilbert Walker)
[b,a] = yulewalk(order,f/(fs/2),m); % frequency must be normalized at fs/2, last value must be 1

% plots
if bplot==1
    [hy,fy] = freqz(b,a,128,fs); % get the magnitude of the yulewalk filter
    semilogx(fy,20*log10(abs(hy)),'linewidth',2)        % yulewalk filter
    hold all
    semilogx(f,20*log10(abs(m)),'k--','linewidth',2)    % ideal filter
    xlabel('frequency in Hz')
    ylabel('Magnitude in dB')
end
end

