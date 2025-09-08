@echo off
echo Building and running theme tests...

REM Create build directory if it doesn't exist
if not exist build mkdir build

REM Change to build directory
cd build

REM Run CMake to generate build files
cmake ..

REM Build the test executable
cmake --build . --config Release

REM Run the theme test executable
ThemeTest.exe

REM Return to the parent directory
cd ..

echo Test execution completed.
pause