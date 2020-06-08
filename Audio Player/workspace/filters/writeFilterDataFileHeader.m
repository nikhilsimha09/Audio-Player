function filename=writeFilterDataFileHeader(path,filtertype,order,info)
filename=path;
% if(strlength(filename) ~= 0)
%     filename=filename+"\\";
% end
filename= filename + filtertype+"_Order"+num2str(order)+".txt";
fileID = fopen(filename,'w');
fprintf(fileID,"%s;%d;%s\n",filtertype,order,info);
fclose(fileID);
end

