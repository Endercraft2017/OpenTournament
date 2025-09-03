#!/bin/bash

# Test build script for OpenTournament

echo "Testing OpenTournament build..."

# Check if required tools are available
if ! command -v cmake &> /dev/null; then
    echo "Error: cmake is not installed"
    exit 1
fi

if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo "Error: No C++ compiler found (g++ or clang++)"
    exit 1
fi

# Create build directory if it doesn't exist
mkdir -p build_test

# Configure the project
echo "Configuring project..."
cmake -S . -B build_test

if [ $? -ne 0 ]; then
    echo "Error: CMake configuration failed"
    exit 1
fi

# Build the project
echo "Building project..."
cmake --build build_test

if [ $? -ne 0 ]; then
    echo "Error: Build failed"
    exit 1
fi

echo "Build test completed successfully!"
echo "To run the application, execute: ./build_test/OpenTournament"