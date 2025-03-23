#!/bin/bash

# Default build type
BUILD_TYPE=${1:-Debug}

echo "Building Underground Beats (${BUILD_TYPE} configuration)"

# Create build directory if it doesn't exist
mkdir -p build || { echo "Failed to create build directory"; exit 1; }

# Change to build directory
cd build || { echo "Failed to change to build directory"; exit 1; }

# Run CMake with specified build type
echo "Configuring CMake..."
cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} .. || { echo "CMake configuration failed"; exit 1; }

# Build the project
echo "Building project..."
cmake --build . --config ${BUILD_TYPE} || { echo "Build failed"; exit 1; }

echo "Build completed successfully!"
