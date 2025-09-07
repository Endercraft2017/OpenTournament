@echo off

REM Build script for OpenTournament on Windows

echo Building OpenTournament...

REM Set Qt6 directory and MinGW directory
set QT_DIR=H:\Qt\6.9.2\mingw_64
set MINGW_DIR=H:\Qt\Tools\mingw1310_64
set CMAKE_PREFIX_PATH=%QT_DIR%

REM Add MinGW to PATH
set PATH=%MINGW_DIR%\bin;%PATH%

REM Create build directory if it doesn't exist
if not exist build mkdir build

REM Configure the project with MinGW generator and Qt6 path
echo Configuring project...
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="%QT_DIR%"

REM Build the project
echo Building project...
cmake --build build

echo Build complete!
echo To run the application, execute: build\OpenTournament.exe
pause