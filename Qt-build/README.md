# hidapi
hidapi 0.9.0+ is required. Luckily, it's available in precompiled form for all 3 platforms in 2020.

## Windows
Download compiled [hidapi package](https://repo.msys2.org/mingw/i686/mingw-w64-i686-hidapi-0.9.0-1-any.pkg.tar.xz) from [msys2 repository](https://packages.msys2.org/package/mingw-w64-i686-hidapi?repo=mingw32), unpack to CommonSense dir ([7zip](https://www.7-zip.org/) can into .tar.xz)

If you want to run a debugging build - use dynamic linking and [x86_64 package](https://repo.msys2.org/mingw/x86_64/mingw-w64-x86_64-hidapi-0.9.0-1-any.pkg.tar.xz) - gdb can't load 32-bit ntdll on Windows 10 :(

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
  *  `Sources` and `MinGW 32-bit` from the latest qt version
  * `Qt Creator` and matching `MinGW 32-bit` from tools
* Fix qt and MinGW versions if needed
* run `build.cmd`
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

Okay, okay. Since there's LOTS of different Linux flavors, let's make it for Ubuntu 16.04.

```
git clone https://github.com/dmaone/CommonSense.git
cd CommonSense/Qt-build
./ubuntu-build.sh
```
You should have FlightController in current directory.

You must run it from root, because by default HID devices only accessible by root :(
Help with "how to allow working with HID devices from normal user" is gladly accepted.
