function writeFilterData(completefilename,fs,b,a)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
if(fopen(completefilename,'r') < 0)
    error("File " + completefilename + ": Write header first (call writeFilterDataFileHeader)!");
end
fileID = fopen(completefilename,'a+');
fprintf(fileID,"%d\n",fs);
for k=1:length(b)
    fprintf(fileID,"%d;",b(k));
end
fprintf(fileID,"\n");
for k=1:length(a)
    fprintf(fileID,"%d;",a(k));
end
fprintf(fileID,"\n");
fclose(fileID);
end

