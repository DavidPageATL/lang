#!/bin/bash

# Build and Run Script for LangProject
# This script configures, builds, and runs the C++ project

set -e  # Exit on any error

echo "=== Building LangProject ==="

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

# Navigate to build directory
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake ..

# Build the project
echo "Building the project..."
make

echo "=== Build completed successfully ==="
echo ""

# Run the executable
echo "=== Running LangProject ==="
./LangProject

echo ""
echo "=== Script completed ==="
