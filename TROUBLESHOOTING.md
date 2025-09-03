# OpenTournament Troubleshooting Guide

## Common Issues and Solutions

### 1. IDE Errors in Header Files

#### Problem

You see red squiggly lines or error messages in your IDE (like VS Code) for Qt includes such as:

- `#include <QApplication>`
- `#include <QMainWindow>`
- `#include <QString>`
- etc.

#### Solution

These are IDE errors, not compilation errors. They occur because your IDE doesn't know where to find the Qt headers. The code will still compile correctly if Qt is properly installed.

To resolve these IDE errors:

**For VS Code:**

1. Install the C/C++ extension by Microsoft
2. Install the Qt-related extensions (optional but helpful)
3. A `.vscode/c_cpp_properties.json` file has been provided in the project to help configure the correct include paths

**Example c_cpp_properties.json:**

```json
{
  "configurations": [
    {
      "name": "Linux",
      "includePath": [
        "${workspaceFolder}/**",
        "/usr/include/qt6",
        "/usr/include/qt6/QtCore",
        "/usr/include/qt6/QtGui",
        "/usr/include/qt6/QtWidgets",
        "/usr/include/qt6/QtSql"
      ],
      "defines": [],
      "compilerPath": "/usr/bin/g++",
      "cStandard": "c17",
      "cppStandard": "c++17",
      "intelliSenseMode": "linux-gcc-x64"
    }
  ],
  "version": 4
}
```

**For Qt Creator:**
Qt Creator should automatically detect Qt installations. If it doesn't:

1. Go to Tools → Options → Kits
2. Check that your Qt version is properly detected
3. Ensure the correct compiler is selected

### 2. Build Errors

#### Problem

CMake configuration fails with messages like:

- `Qt6 not found`
- `Could not find Qt6CoreConfig.cmake`

#### Solution

This means Qt6 is not properly installed or not in your PATH.

**On Ubuntu/Debian:**

```bash
sudo apt update
sudo apt install qt6-base-dev qt6-tools-dev-tools
```

**On Fedora:**

```bash
sudo dnf install qt6-qtbase-devel qt6-qttools-devel
```

**On Windows:**

1. Download and install Qt6 from https://www.qt.io/download
2. Make sure to include the Qt6 development libraries
3. Add Qt6 bin directory to your PATH

**On macOS:**

```bash
brew install qt6
```

### 3. Database Issues

#### Problem

Application fails to start with database errors.

#### Solution

The application creates a `tournament.db` file in the application directory. Make sure:

1. The application has write permissions to its directory
2. The directory is not read-only
3. There's sufficient disk space

### 4. Runtime Issues

#### Problem

Application crashes or behaves unexpectedly at runtime.

#### Solution

1. Check that all required Qt6 DLLs are available (especially on Windows)
2. Run the application from the command line to see any error messages
3. Ensure the `tournament.db` file is not corrupted (you can delete it to start fresh)

### 5. Building the Application

#### Problem

Build fails with compiler errors.

#### Solution

1. Ensure you're using a C++17 compatible compiler
2. Check that CMake version is 3.16 or higher
3. Verify all source files are present and readable

**To build manually:**

```bash
mkdir build
cd build
cmake ..
make # or cmake --build .
```

### 6. Export Functionality Issues

#### Problem

CSV export fails or produces incorrect files.

#### Solution

1. Ensure the application has write permissions to the target directory
2. Check that the target directory exists
3. Verify that the file path doesn't contain invalid characters

### 7. Pairing Algorithm Issues

#### Problem

Pairings are not generated correctly.

#### Solution

1. Ensure you have at least 3 players added
2. Check that player names are unique
3. Restart the application to regenerate pairings if needed

## Testing the Installation

To verify that everything is working correctly:

1. **Test Build:**

   ```bash
   ./test_build.sh  # Linux/macOS
   test_build.bat   # Windows
   ```

2. **Run the Application:**

   ```bash
   ./build/OpenTournament  # Linux/macOS
   build\Debug\OpenTournament.exe  # Windows
   ```

3. **Basic Functionality Test:**
   - Add 4 players
   - Verify pairings are generated
   - Submit a match result
   - Check that the leaderboard updates
   - Export results to CSV

## Getting Help

If you're still experiencing issues:

1. Check the console output for error messages
2. Verify all prerequisites are installed
3. Try building in a clean directory
4. Consult the Qt documentation for your platform
5. Report issues with detailed error messages and steps to reproduce

## System Requirements

- **Operating System:** Windows 7+, macOS 10.12+, or Linux
- **Qt Version:** Qt6
- **CMake:** 3.16 or higher
- **Compiler:** C++17 compatible (GCC 7+, Clang 5+, MSVC 2017+)
- **Disk Space:** 100MB free space
