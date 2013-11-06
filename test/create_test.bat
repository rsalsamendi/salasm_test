set AS=%1
set OUTPUT=%2
set INPUT=%3

%AS% -DARCH=16 --oformat=bin --objfile=%OUTPUT%-16.bin %INPUT%
%AS% -DARCH=32 --oformat=bin --objfile=%OUTPUT%-32.bin %INPUT%
%AS% -DARCH=64 --oformat=bin --objfile=%OUTPUT%-64.bin %INPUT%
