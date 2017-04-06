=hidapi build=

First, you'll need to build HIDAPI lib.

==Windows==
you need msys32.
https://msys2.github.io/

https://wiki.qt.io/MSYS2 is a good guide.

!!!WARNING!!! you need "msys MinGW 32 bit" shell !!!

Pre-requisites:

pacman -S autoconf automake libtool gcc

=mac=
You'll need brew - https://brew.sh
Open terminal,
```
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

brew install autoconf automake libtool
```

=for all systems=

navigate one level above CommonSense dir.

git clone https://github.com/signal11/hidapi.git

./bootstrap
./configure
make

you should have libhidapi-0.dll in [platform]/.libs - leave it there.


=Windows build=

Because windows app can be a single file - static version is better.
==Building static Qt==

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


=OS X build=
Since OS X application is a folder anyway - no point in making static version.
http://download.qt.io/official_releases/qt/

get and install qt-opensource-mac-x64-clang*
Unfortunately, you'll need xcode. Which is additional 5GB.
If you 

brew install qt
brew install Caskroom/cask/qt-creator

There will be no kits. You'll need to add Qt version (Cmd-Shift-G, /usr/local) - opt/qt/bin/qmake - then save. 
Qt creator will crash. Open again, go kit selection, select Qt version. You'll have "Desktop" build.
Then you'll have "Error: Could not resolve SDK path for 'macosx' and that would be it.
Because Qt5 requires xcode "because we've got support requests from people using frankenstein buildchains".

