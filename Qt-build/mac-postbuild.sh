#!/bin/sh
QTVER=5.8.0
COMPILER=clang_64
BUILD_DIR=../build-FlightController-Desktop_Qt_5_8_0_${COMPILER}bit-Release/
pushd $BUILD_DIR
/Users/dma/Qt5.8.0/5.8/${COMPILER}/bin/macdeployqt FlightController.app -verbose=2 -dmg
popd
mv $BUILD_DIR/FlightController.dmg .
