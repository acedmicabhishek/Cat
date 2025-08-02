#!/bin/bash

set -e

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

# Configure and build
cd build
cmake ..
make

# Run the compiler and print the LLVM IR
./cat ../test/main.cat