# hidapi build

First, you'll need to build HIDAPI lib.

## Windows
you need [msys32](https://msys2.github.io/). Here's [a good guide](https://wiki.qt.io/MSYS2).

!!!WARNING!!! you need "msys MinGW 32 bit" shell !!!

```
pacman -S autoconf automake libtool gcc
```

## mac
You'll need [brew](https://brew.sh)
Open terminal,
```
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

brew install autoconf automake libtool
```

## Both

navigate to CommonSense dir.

```
git clone https://github.com/signal11/hidapi.git

./bootstrap
./configure
make
```

you should have libhidapi[something] in [platform]/.libs - leave it there.


## Ubuntu 16.04
```
sudo apt-get install libhidapi-dev libhidapi-hidraw0
```

# Windows build

Because windows app can be a single file - static version is better.
## Building static Qt

*Exclude C:\Qt from antivirus scanning. Source install takes forever if you don't.*

[Official way](https://wiki.qt.io/Building_a_static_Qt_for_Windows_using_MinGW)

### Abridged version
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

# OS X build
Since OS X application is a folder anyway - no point in making static version.

You'll need an official Qt, brew version doesn't work.

http://download.qt.io/official_releases/qt/

get and install qt-opensource-mac-x64-clang*

Unfortunately, you'll need xcode. But not from appstore - officially you need XCode 5.0.0, 5.1 works too.

Anyway. Install Qt, install XCode.

The following is for people who try to walk this path elsewhere - mac-build.sh takes care of this.

## DETOUR
Then you'll have "Error: Could not resolve SDK path for 'macosx' and that would be it.

Because Qt5 requires xcode "because we've got support requests from people using frankenstein buildchains".

```
sudo xcode-select -s /Applications/Xcode.app/Contents/Developer
xcodebuild
```
xcodebuild will require you to accept license. If it complains about no XCode project - you already accepted the license, good!

You have working Qt environment now.

## /DETOUR

```
git clone https://github.com/dmaone/CommonSense.git
cd CommonSense/Qt-build
./mac-build.sh
```
You should have FlightController.dmg in this dir. Congratulations.

# Linux build
Linux users are supposed to know how to do that :)

Okay, okay. Since there's LOTS of different Linux flavors, let's make it for Ubuntu 16.04.

```
git clone https://github.com/dmaone/CommonSense.git
cd CommonSense/Qt-build
./ubuntu-build.sh
```
You should have FlightController in current directory.

You must run it from root, because by default HID devices only accessible by root :(
Help with "how to allow working with HID devices from normal user" is gladly accepted.