
cd %SRCDIR%

configure.bat -platform win32-g++ -prefix c:\qt\static -opensource -confirm-license ^
 -static-runtime -static -release -feature-designer ^
 -qt-freetype  -qt-libjpeg -qt-pcre -qt-libpng -qt-zlib -opengl desktop ^
 -no-feature-sql -no-feature-sqlmodel -no-sql-odbc -no-sql-sqlite ^
 -no-openssl ^
 -skip qt5compat -skip qtdeclarative -skip qtconnectivity -skip qtdatavis3d -skip qtnetworkauth -skip qtpositioning ^
 -skip qtlanguageserver -skip qttranslations -skip qtcoap -skip qtactiveqt ^
 -skip qtlocation -skip qtmultimedia -skip qtremoteobjects -skip qtsensors -skip qtserialbus -skip qtserialport -skip qtwebengine ^
 -skip qtcharts -skip qthttpserver -skip qdoc -skip qttools ^
 -skip qt3d -skip qtquick3d -skip qtquickcontrols -skip qtquick3dphysics -skip qtquickcontrols2 -skip qtquicktimeline ^
 -skip qtdoc -skip qtlottie -skip qtmqtt -skip qtopcua -skip qtscxml -skip qtshadertools -skip qtspeech -skip qtsvg ^
 -skip qtvirtualkeyboard -skip qtwayland -skip qtwebchannel -skip qtwebsockets -skip qtwebview ^
 -make tools -nomake examples -nomake tests
