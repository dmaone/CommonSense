SET QTVER=5.15.2
SET SRCDIR=c:\qt\%QTVER%\Src

SET MINGWTOOLS=c:\qt\tools\mingw810_32
SET MINGW=c:\qt\%QTVER%\mingw81_32



SET PATH=%SRCDIR%\qtbase\bin;%SRCDIR%\gnuwin32\bin;%MINGWTOOLS%\bin;%MINGW%\bin;%PATH%

cd %SRCDIR%

CALL configure.bat -platform win32-g++ -prefix c:\qt\static -opensource -confirm-license ^
 -static-runtime -static -release -feature-designer -native-win32-bluetooth ^
 -qt-freetype  -qt-libjpeg -qt-pcre -qt-libpng -qt-zlib -opengl desktop -feature-qdoc ^
 -no-feature-sql -no-feature-sqlmodel -no-sql-odbc -no-sql-sqlite ^
 -no-feature-network -no-openssl ^
 -skip qtlocation -skip qtwebengine -no-feature-qdoc ^
 -skip qt3d -skip qtquick3d -skip qtquickcontrols -skip qtquickcontrols2 -skip qtquicktimeline ^
 -make libs -make tools -nomake examples -nomake tests

mingw32-make -k -j4
mingw32-make -k install
