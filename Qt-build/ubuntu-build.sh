#!/bin/bash

sudo apt-get install libhidapi-dev libhidapi-hidraw0 qt5-default gcc
mkdir build-ubuntu
pushd build-ubuntu
qmake ../../FlightController/FlightController.pro -r -spec linux-g++-64
make
cp -af FlightController ..
popd
