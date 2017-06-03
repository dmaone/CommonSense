#!/bin/sh

# Copied from http://andrewdolby.com/articles/2016/continuous-deployment-for-qt-applications/

set -o errexit -o nounset

# Update platform
echo "Updating platform..."

# Install p7zip for packaging archive for deployment
sudo -E apt-get -yq --no-install-suggests --no-install-recommends --force-yes install p7zip-full

# Need to install chrpath to set up proper rpaths. This is necessary
# to allow Qt libraries to be visible to each other. Alternatively,
# you could use qt.conf, which wouldn't require chrpath.
sudo -E apt-get -yq --no-install-suggests --no-install-recommends --force-yes install chrpath

# Hold on to current directory
project_dir=$(pwd)

# Define your Qt install path for later use
qt_install_dir=/opt

# Install Qt
echo "Installing Qt..."
cd ${qt_install_dir}
echo "Downloading Qt files..."
sudo wget https://github.com/adolby/qt-more-builds/releases/download/5.7/qt-opensource-5.7.0-linux-x86_64.7z
echo "Extracting Qt files..."
sudo 7z x qt-opensource-5.7.0-linux-x86_64.7z &> /dev/null

# Install Qt Installer Framework
echo "Installing Qt Installer Framework..."
sudo wget https://github.com/adolby/qt-more-builds/releases/download/qt-ifw-2.0.3/qt-installer-framework-opensource-2.0.3-linux.7z
sudo 7z x qt-installer-framework-opensource-2.0.3-linux.7z &> /dev/null

# Add Qt binaries to path
echo "Adding Qt binaries to path..."
PATH=${qt_install_dir}/Qt/5.7/gcc_64/bin/:${qt_install_dir}/Qt/QtIFW2.0.3/bin/:${PATH}

# Build YourApp
echo "Building FlightController..."
cd ${project_dir}

# Output qmake version info to make sure we have the right install
# directory in the PATH variable
qmake -v

qmake -config release FlightController/FlightController.pro -spec linux-g++-64
make

# Build and run your tests here

# Package YourApp
echo "Packaging FlightController..."
cd ${project_dir}/build/linux/gcc/x86_64/release/FlightController/

# Remove build directories that you don't want to deploy
rm -rf moc
rm -rf obj
rm -rf qrc

echo "Copying files for archival..."

# Copy ICU libraries
cp "${qt_install_dir}/Qt/5.7/gcc_64/lib/libicui18n.so.56.1" "libicui18n.so.56"
cp "${qt_install_dir}/Qt/5.7/gcc_64/lib/libicuuc.so.56.1" "libicuuc.so.56"
cp "${qt_install_dir}/Qt/5.7/gcc_64/lib/libicudata.so.56.1" "libicudata.so.56"

# Copy YourApp's Qt dependencies, including QML dependencies if your
# app uses QML. If your app doesn't use QML, you'll probably need to
# include libQt5Widgets.so.5. You will need additional library
# dependency files if your app uses Qt features found in other
# modules. You can find out your app's library and QML dependencies by
# checking the Qt docs or by referencing the library and QML files
# that are copied by macdeployqt or windeployqt on macOS or Windows.
mkdir platforms
mkdir -p Qt/labs/

# You'll always need these libraries on Linux.
cp "${qt_install_dir}/Qt/5.7/gcc_64/plugins/platforms/libqxcb.so" "platforms/libqxcb.so"
cp "${qt_install_dir}/Qt/5.7/gcc_64/plugins/platforms/libqminimal.so" "platforms/libqminimal.so"
cp "${qt_install_dir}/Qt/5.7/gcc_64/lib/libQt5Core.so.5.7.0" "libQt5Core.so.5"
cp "${qt_install_dir}/Qt/5.7/gcc_64/lib/libQt5Gui.so.5.7.0" "libQt5Gui.so.5"
cp "${qt_install_dir}/Qt/5.7/gcc_64/lib/libQt5DBus.so.5.7.0" "libQt5DBus.so.5"
cp "${qt_install_dir}/Qt/5.7/gcc_64/lib/libQt5XcbQpa.so.5.7.0" "libQt5XcbQpa.so.5"
cp "${qt_install_dir}/Qt/5.7/gcc_64/lib/libQt5Widgets.so.5.7.0" "libQt5Widgets.so.5"

# You may or may not need these, depending on which Qt features you
# use
#cp "${qt_install_dir}/Qt/5.7/gcc_64/lib/libQt5Svg.so.5.7.0" "libQt5Svg.so.5"
#cp "${qt_install_dir}/Qt/5.7/gcc_64/lib/libQt5Qml.so.5.7.0" "libQt5Qml.so.5"
#cp "${qt_install_dir}/Qt/5.7/gcc_64/lib/libQt5Quick.so.5.7.0" "libQt5Quick.so.5"
#cp "${qt_install_dir}/Qt/5.7/gcc_64/lib/libQt5QuickControls2.so.5.7.0" "libQt5QuickControls2.so.5"
#cp "${qt_install_dir}/Qt/5.7/gcc_64/lib/libQt5QuickTemplates2.so.5.7.0" "libQt5QuickTemplates2.so.5"
#cp "${qt_install_dir}/Qt/5.7/gcc_64/lib/libQt5Network.so.5.7.0" "libQt5Network.so.5"

cp -R "${qt_install_dir}/Qt/5.7/gcc_64/qml/Qt/labs/settings/" "Qt/labs/"
cp -R "${qt_install_dir}/Qt/5.7/gcc_64/qml/QtGraphicalEffects/" "."
cp -R "${qt_install_dir}/Qt/5.7/gcc_64/qml/QtQuick/" "."
cp -R "${qt_install_dir}/Qt/5.7/gcc_64/qml/QtQuick.2/" "."

# Copy other project files
cp "${project_dir}/README.md" "README.md"
cp "${project_dir}/LICENSE" "LICENSE"
cp "${project_dir}/Qt License" "Qt License"

# Use chrpath to set up rpaths for Qt's libraries so they can find
# each other
chrpath -r \$ORIGIN/.. platforms/libqxcb.so
chrpath -r \$ORIGIN/.. platforms/libqminimal.so
chrpath -r \$ORIGIN/../.. QtQuick/Controls.2/libqtquickcontrols2plugin.so
chrpath -r \$ORIGIN/../.. QtQuick/Templates.2/libqtquicktemplates2plugin.so

# Copy files to a folder with configuration for creating an installer
# with Qt Installer Framework
echo "Copying files for installer..."
cp -R * "${project_dir}/installer/linux/packages/com.dma.flightcontroller/data/"

# Copy files to a portable archive inside the containing folder
# created earlier
echo "Packaging portable archive..."
cd ..
7z a FlightController_${TAG_NAME}_linux_x86_64_portable.zip FlightController

# Use the Qt Installer Framework to create an installer executable
echo "Creating installer..."
cd "${project_dir}/installer/linux/"
binarycreator --offline-only -c config/config.xml -p packages FlightController_${TAG_NAME}_linux_x86_64_installer

echo "Done!"

exit 0