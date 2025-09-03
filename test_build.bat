@echo off

REM Test build script for OpenTournament on Windows

echo Testing OpenTournament build...

REM Check if required tools are available
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: cmake is not installed or not in PATH
    pause
    exit /b 1
)

where cl >nul 2>&1
if %errorlevel% neq 0 (
    echo Warning: Microsoft Visual C++ compiler not found, checking for MinGW...
    where g++ >nul 2>&1
    if %errorlevel% neq 0 (
        echo Error: No C++ compiler found (MSVC or MinGW g++)
        pause
        exit /b 1
    )
)

REM Create build directory if it doesn't exist
if not exist build_test mkdir build_test

REM Configure the project
echo Configuring project...
cmake -S . -B build_test

if %errorlevel% neq 0 (
    echo Error: CMake configuration failed
    pause
    exit /b 1
)

REM Build the project
echo Building project...
cmake --build build_test

if %errorlevel% neq 0 (
    echo Error: Build failed
    pause
    exit /b 1
)

echo Build test completed successfully!
echo To run the application, execute: build_test\Debug\OpenTournament.exe
pause