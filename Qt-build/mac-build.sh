#!/bin/bash

QT_VERSION=5.9
BUILD_DIR=build-macx

QT_PACKAGE=qt
brew install $QT_PACKAGE

export PATH="$(brew --prefix $QT_PACKAGE)/bin:$PATH"

echo "Preparing icon set"
pushd ../FlightController-icons
mkdir FlightController.iconset
cd FlightController.iconset
cp -a ../icon_* .
cp -f icon_1024x1024.png icon_512x512@2x.png
cp -f icon_512x512.png icon_256x256@2x.png
cp -f icon_256x256.png icon_128x128@2x.png
cp -f icon_64x64.png icon_32x32@2x.png
cp -f icon_32x32.png icon_16x16@2x.png
cd ..
iconutil -c icns FlightController.iconset
cp -a FlightController.icns ../FlightController
popd
echo "About to switch XCode to the one in /Applications. XCode MUST BE INSTALLED!!!"
sudo xcode-select -s /Applications/Xcode.app/Contents/Developer
echo "now running xcodebuild - you need to accept license for Qt to resolve SDK path!!!"
xcodebuild
echo "If xcodebuild returns error - it's okay! Means it's working."

test -d $BUILD_DIR && rm -rf $BUILD_DIR
mkdir $BUILD_DIR
pushd $BUILD_DIR
# so that macdeployqt may find libhidapi
cp ../../../hidapi/mac/.libs/libhidapi.0.dylib /usr/local/lib
qmake ../../FlightController/FlightController.pro -r -spec macx-clang
make && macdeployqt FlightController.app -verbose=2 -dmg
popd
mv -f $BUILD_DIR/FlightController.dmg .

