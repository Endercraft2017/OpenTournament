#!/bin/bash

# Build script for OpenTournament

echo "Building OpenTournament..."

# Create build directory if it doesn't exist
mkdir -p build

# Configure the project
echo "Configuring project..."
cmake -S . -B build

# Build the project
echo "Building project..."
cmake --build build

echo "Build complete!"
echo "To run the application, execute: ./build/OpenTournament"