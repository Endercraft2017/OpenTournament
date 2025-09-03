# CMakeLists.txt Specification for OpenTournament

## Overview

This document specifies the CMake configuration for the OpenTournament application, which uses Qt6 and requires C++17 support.

## Required Components

- CMake 3.16 or higher
- Qt6 Core module
- Qt6 Gui module
- Qt6 Widgets module
- Qt6 Sql module

## CMakeLists.txt Content

```cmake
# Set minimum required CMake version
cmake_minimum_required(VERSION 3.16)

# Project declaration
project(OpenTournament VERSION 0.1 LANGUAGES CXX)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)

# Add executable with all source files
add_executable(OpenTournament
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/Database.cpp
    src/Database.h
    src/Player.cpp
    src/Player.h
    src/Match.cpp
    src/Match.h
    src/Tournament.cpp
    src/Tournament.h
)

# Link Qt6 libraries to the executable
target_link_libraries(OpenTournament PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql)

# Install rules for packaging
install(TARGETS OpenTournament DESTINATION bin)
install(FILES tournament.db DESTINATION share/OpenTournament)
```

## Build Instructions

### Configure the project:

```bash
cmake -S . -B build
```

### Build the project:

```bash
cmake --build build
```

### Run the binary:

```bash
./build/OpenTournament
```

## Windows Packaging Notes

On Windows, after building, use `windeployqt` to bundle the required Qt DLLs:

```bash
windeployqt build/OpenTournament.exe
```

This will copy the necessary Qt DLLs to the build directory, making the application portable.
