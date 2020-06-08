function [b,a]=createBasicDelayFilters(gFF,gFB,tdelay,fs)
Ts=1/fs;                    % sample duration
order=round(tdelay/Ts)+1;   % filter order          
if gFB==0
    b=[1 zeros(1,order-1) gFF];   % numerator coefficients
    a=1;
    filename=writeFilterDataFileHeader("..\files\",['basic_delay_' num2str(fs)],order,...
                "delay time td="+ num2str(tdelay*1000) +"ms Gain gFF=" + num2str(gFF));
else
    b=[1 zeros(1,order-1) (gFF-gFB)];   % numerator coefficients
    a=[1 zeros(1,order-1) -gFB];        % denominator coefficients
    filename=writeFilterDataFileHeader("..\files\",['basic_delay_feedback_' num2str(fs)],order,...
    "delay time td="+ num2str(tdelay*1000) +"ms foreward gain gFF=" + num2str(gFF) + "feedback gain gFB=" + num2str(gFB));
end
% write the filter coefficients
writeFilterData(filename,fs,b,a);
end
