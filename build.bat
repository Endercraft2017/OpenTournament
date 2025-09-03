@echo off

REM Build script for OpenTournament on Windows

echo Building OpenTournament...

REM Create build directory if it doesn't exist
if not exist build mkdir build

REM Configure the project
echo Configuring project...
cmake -S . -B build

REM Build the project
echo Building project...
cmake --build build

echo Build complete!
echo To run the application, execute: build\Debug\OpenTournament.exe
pause