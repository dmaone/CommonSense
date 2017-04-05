Building static Qt under Windows 10:

!!! Exclude C:\Qt from antivirus scanning. Source install takes forever if you don't.

Official way: https://wiki.qt.io/Building_a_static_Qt_for_Windows_using_MinGW

http://download.qt.io/official_releases/qt/

Get qt-opensource-windows-x86-mingw?????.exe
Install EVERYTHING to C:\Qt.
Perl msi didn't run on me - installed with defaults to c:\Strawberry.


```

cd \qt
mkdir static
mv Src static
mkdir static\5.8\mkspecs\win32-g++

```

Fix versions in windows-build-qt-static.ps1

Run powershell as administrator. Run windows-build-qt-static.ps1 - you'll need to Set-Ex[TAB] Bypass to run it.

This will take a while.


Then do "Qt Creator setup" - add _static_ version, not the on that's there.
Don't forget to click "Apply".

"Unnamed" kit is automatically added, rename, select static version for qt version.


-=-

hidapi build

you need msys32.
https://msys2.github.io/

https://wiki.qt.io/MSYS2 is a good guide.

!!!WARNING!!! you need "msys MinGW 32 bit" shell !!!

Pre-requisites:

pacman -S autoconf automake libtool gcc


navigate one level above CommonSense dir.

git clone https://github.com/signal11/hidapi.git

./bootstrap
./configure
make

you should have libhidapi-0.dll in windows/.libs - leave it there.

