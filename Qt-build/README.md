# hidapi
hidapi 0.9.0+ is required. Luckily, it's available in precompiled form for all 3 platforms in 2020.

## Windows
Download compiled [hidapi package](https://mirror.msys2.org/mingw/mingw32/mingw-w64-i686-hidapi-0.13.1-2-any.pkg.tar.zst) from [msys2 repository](https://packages.msys2.org/package/mingw-w64-i686-hidapi?repo=mingw32), unpack to CommonSense dir (you'll need [7zip-zstd](https://github.com/mcmilk/7-Zip-zstd/releases))
NOTE: There's precompiled DLL in [hidapi repo](https://github.com/libusb/hidapi/releases) - but you'll need libhidapi.a for static linking, DLL won't help.

If you want to run a debugging build - use dynamic linking and [x86_64 package](https://mirror.msys2.org/mingw/mingw64/mingw-w64-x86_64-hidapi-0.13.1-2-any.pkg.tar.zst) - gdb can't load 32-bit ntdll on Windows 10 :(

NOTE: for qt6 you'll also need x86_64 package, because qt6 no longer supports 32-bit.

## mac
You'll need [brew](https://brew.sh)
Open terminal,
```
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

brew install hidapi
```


## Ubuntu 16.04
```
sudo apt-get install libhidapi-dev libhidapi-hidraw0
```

# Windows build

Because windows app can be a single file - static version is better.
## Building static Qt

*Exclude C:\Qt from antivirus scanning. Source install takes forever if you don't.*

[Official way](https://wiki.qt.io/Building_a_static_Qt_for_Windows_using_MinGW)

### Walkthrough
* Download and run [opensource QT installer](https://www.qt.io/download-open-source)
* Install the following into `c:\qt`
  * `Sources` from the latest qt version
  * `MinGW 32-bit` for qt5, `MinGW 64-bit` for qt6 (no more 32-bit)
  * `Qt Creator` and matching `MinGW` from tools
  * qt6-only: `Cmake` and `Ninja` from tools
* Fix qt and MinGW versions if needed
* qt6-only
  * install python
  * switch win32 debug mode paths from 32-bit to 64-bit.
* run `build.cmd` (`qt6build.cmd` for qt6)
* https://www.youtube.com/watch?v=bMVbaCiy_XE
* Profit!

Run Qt Creator, Tools->options, select "Build & Run", add new Qt version from `c:\Qt\Static\`
then clone autodetected kit, name it `static`, select newly added Qt version for it.

Don't forget to click "Apply".

Now it's time to open the project. Select `static` kit in setup dialog. In bottom-left corner, select static kit, release config.

Click the green "Run" icon. Find `FlightController.exe` in the build directory - you can copy it and use to control your keyboard.

program crashes, debug run says "During startup program exited with code 0x00000135"? You haven't selected the static kit.


# OS X build
Since OS X application is a folder anyway - no point in making static version. One can use qt from brew.

You'll need XCode. Look at the previous version of this document if the one from appstore doesn't work for you.

The detour section is for people who try to walk this path elsewhere - `mac-build.sh` takes care of this.

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

Okay, okay. Since there's LOTS of different Linux flavors, let's make it for Ubuntu 16.04. (NOTE: doesn't work for ubuntu 22.04)

```
git clone https://github.com/dmaone/CommonSense.git
cd CommonSense/Qt-build
./ubuntu-build.sh
```
You should have FlightController in current directory.

You must run it from root, because by default HID devices only accessible by root :(
Help with "how to allow working with HID devices from normal user" is gladly accepted.
