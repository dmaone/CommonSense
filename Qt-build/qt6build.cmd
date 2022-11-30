@ECHO OFF
SET QTVER=6.4.1
SET SRCDIR=c:\qt\%QTVER%\Src

SET TOOLS=c:\qt\tools
SET MINGWTOOLS=c:\qt\tools\mingw1120_64
SET MINGW=c:\qt\%QTVER%\mingw1120_64

SET PATH=%SRCDIR%\qtbase\bin;%TOOLS%\Cmake_64\bin;%TOOLS%\Ninja;%MINGW%\bin;%MINGWTOOLS%\bin;%PATH%
echo START
CALL _configure.cmd
echo FINISH
cd %SRCDIR%
cmake --build . --parallel
cmake --install .
echo YAY