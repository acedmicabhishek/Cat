#!/bin/bash

set -ex

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

# Configure and build
cd build
echo "Configuring with CMake..."
cmake ..
echo "Building with make..."
make

# Run the compiler to generate LLVM IR
echo "Generating LLVM IR..."
./cat ../test/main.cat

# Compile the LLVM IR with clang
echo "Compiling LLVM IR with clang..."
clang output.ll -o my_program

# Run the compiled program
echo "Running the compiled program..."
./my_program