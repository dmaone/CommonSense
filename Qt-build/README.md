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
cd hidapi
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

Get qt-opensource-windows-x86-???.exe from a version subdir. Latest is 5.14.1 as of now.
Install "Sources", "MinGWx.x.x 32bit" and everything from "Tools" to C:\Qt.
Perl msi didn't run on me - installed with defaults to c:\Strawberry.
copy windows-build-qt-static.ps1 to c:\qt
```

cd \qt
mkdir static
mv 5.14.1\Src static

```

Fix versions in windows-build-qt-static.ps1

Run powershell as administrator. Run windows-build-qt-static.ps1 - you _may_ need to `Set-Ex[TAB] Bypass` to run it.

This will take a while.

Then in Qt Creator go Tools->options, select "Build & Run", add new Qt version from c:\Qt\Static\{qt version}\, 
then clone autodetected kit, name "static", select newly added Qt version for it.

Don't forget to click "Apply".

Now it's time to open the project. Select "static" kit in setup dialog. In bottom-left corner, select static kit, release config.

Click the green "Run" icon. Find FlightController.exe in the build directory - you can copy it and use to control your keyboard.

program crashes, debug run says "During startup program exited with code 0x00000135"? You haven't selected the static kit.


# OS X build
Since OS X application is a folder anyway - no point in making static version. One can use qt from brew.

You'll need XCode. Look at the previous version of this document if the one from appstore doesn't work for you.

The detour section is for people who try to walk this path elsewhere - mac-build.sh takes care of this.

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

From the same path you were before building hidapi (i.e. you should have "hidapi/mac/.libs" accessible from it):
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
