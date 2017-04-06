#!/bin/sh

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
