# Build and Packaging Configuration Specification

## Overview

This document specifies the build and packaging configuration for the OpenTournament application. It covers CMake configuration, build processes, and packaging for distribution.

## CMake Configuration

### Minimum Requirements

- **CMake Version**: 3.16 or higher
- **C++ Standard**: C++17
- **Qt Version**: Qt6

### Project Declaration

```cmake
cmake_minimum_required(VERSION 3.16)
project(OpenTournament VERSION 0.1 LANGUAGES CXX)
```

### C++ Standard Settings

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

### Qt6 Components

```cmake
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)
```

### Source Files

```cmake
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/Database.cpp
    src/Player.cpp
    src/Match.cpp
    src/Tournament.cpp
)

set(HEADERS
    src/MainWindow.h
    src/Database.h
    src/Player.h
    src/Match.h
    src/Tournament.h
)
```

### Executable Definition

```cmake
add_executable(OpenTournament ${SOURCES} ${HEADERS})
```

### Library Linking

```cmake
target_link_libraries(OpenTournament PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql)
```

### Install Rules

```cmake
install(TARGETS OpenTournament DESTINATION bin)
install(FILES tournament.db DESTINATION share/OpenTournament)
```

## Build Process

### Configuration Step

```bash
cmake -S . -B build
```

### Build Step

```bash
cmake --build build
```

### Build Types

- **Debug**: Default for development
- **Release**: Optimized for distribution

### Compiler Options

```cmake
# Debug build
set(CMAKE_BUILD_TYPE Debug)

# Release build
set(CMAKE_BUILD_TYPE Release)
```

## Platform-Specific Configuration

### Windows

- **Compiler**: MSVC or MinGW
- **Packaging Tool**: windeployqt
- **File Extension**: .exe

#### Windows Packaging

```bash
# Build the application
cmake --build build --config Release

# Deploy Qt libraries
windeployqt build/Release/OpenTournament.exe
```

### Linux

- **Compiler**: GCC
- **Packaging**: tar.gz or DEB/RPM packages
- **File Extension**: None (executable)

#### Linux Packaging

```bash
# Build the application
cmake --build build

# Create tar.gz package
tar -czvf OpenTournament-0.1-Linux.tar.gz build/OpenTournament tournament.db
```

### macOS

- **Compiler**: Clang
- **Packaging**: DMG or PKG installer
- **File Extension**: .app

#### macOS Packaging

```bash
# Build the application
cmake --build build

# Create app bundle (additional CMake configuration needed)
```

## Dependencies

### Runtime Dependencies

- **Qt6 Libraries**:
  - QtCore
  - QtGui
  - QtWidgets
  - QtSql
- **SQLite**: Usually included with QtSql

### Development Dependencies

- **CMake**: 3.16+
- **C++ Compiler**: Supporting C++17
- **Qt6 Development Libraries**

## Directory Structure

### Source Distribution

```
OpenTournament/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── main.cpp
│   ├── MainWindow.cpp
│   ├── MainWindow.h
│   ├── Database.cpp
│   ├── Database.h
│   ├── Player.cpp
│   ├── Player.h
│   ├── Match.cpp
│   ├── Match.h
│   ├── Tournament.cpp
│   └── Tournament.h
├── resources/
└── tournament.db
```

### Build Directory

```
build/
├── OpenTournament (or OpenTournament.exe)
├── Qt6 DLLs (Windows only)
├── Qt platform plugins/
└── Other Qt dependencies
```

### Installation Directory

```
/usr/local/ (Linux) or C:\Program Files\OpenTournament\ (Windows)
├── bin/
│   └── OpenTournament
├── share/
│   └── OpenTournament/
│       └── tournament.db
└── lib/ (if applicable)
```

## Packaging Process

### Windows Packaging Steps

1. Build Release configuration
2. Run windeployqt to bundle Qt libraries
3. Create installer or ZIP package
4. Include README and license files

### Linux Packaging Steps

1. Build application
2. Create tar.gz archive
3. Optionally create DEB/RPM packages
4. Include README and license files

### macOS Packaging Steps

1. Build application
2. Create app bundle
3. Create DMG installer
4. Include README and license files

## Version Management

### Version Information

- **Major**: 0 (Initial development)
- **Minor**: 1 (First release)
- **Patch**: 0 (Initial patch)

### CMake Version Handling

```cmake
project(OpenTournament VERSION 0.1.0 LANGUAGES CXX)

# Access version components
message(STATUS "OpenTournament version: ${OpenTournament_VERSION}")
message(STATUS "Major: ${OpenTournament_VERSION_MAJOR}")
message(STATUS "Minor: ${OpenTournament_VERSION_MINOR}")
message(STATUS "Patch: ${OpenTournament_VERSION_PATCH}")
```

## Build Automation

### Continuous Integration

- **Platform**: GitHub Actions, GitLab CI, or similar
- **Matrix**: Test on Windows, Linux, and macOS
- **Steps**:
  1. Install dependencies
  2. Configure CMake
  3. Build application
  4. Run tests (if any)
  5. Create packages

### Release Process

1. Update version in CMakeLists.txt
2. Create Git tag
3. Build for all platforms
4. Create and upload packages
5. Update release notes

## Testing Build Configurations

### Cross-Platform Testing

- Verify builds on Windows, Linux, and macOS
- Test both Debug and Release configurations
- Verify dependency bundling works correctly

### Dependency Verification

- Check that all required libraries are included
- Verify application runs without installation
- Test on clean systems without Qt installed

## Troubleshooting

### Common Issues

1. **Qt Libraries Not Found**:

   - Ensure Qt6 is installed
   - Check CMAKE_PREFIX_PATH
   - Verify Qt6_DIR environment variable

2. **CMake Version Too Old**:

   - Update CMake to 3.16 or higher
   - Use system package manager or download from cmake.org

3. **C++17 Not Supported**:
   - Update compiler
   - Check compiler documentation for C++17 support

### Platform-Specific Issues

- **Windows**: Path length limitations, Visual Studio version
- **Linux**: Library paths, package manager dependencies
- **macOS**: Xcode command line tools, Homebrew Qt installation

## Documentation

### Build Instructions

Include clear build instructions in README.md:

```markdown
## Building from Source

### Prerequisites

- CMake 3.16 or higher
- Qt6 development libraries
- C++ compiler with C++17 support

### Build Steps

1. Configure: `cmake -S . -B build`
2. Build: `cmake --build build`
3. Run: `./build/OpenTournament`
```

### Packaging Instructions

Include packaging instructions for maintainers:

```markdown
## Creating Packages

### Windows

1. Build Release configuration
2. Run `windeployqt build/OpenTournament.exe`
3. Create ZIP package with all files

### Linux

1. Build application
2. Create tar.gz archive
3. Optionally create DEB/RPM packages
```
