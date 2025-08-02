cmake_minimum_required(VERSION 3.13)
project(Cat)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find llvm-config, the most reliable way to get LLVM flags
find_program(LLVM_CONFIG llvm-config
    DOC "Path to llvm-config executable"
)
if(NOT LLVM_CONFIG)
    message(FATAL_ERROR "llvm-config not found. Please install LLVM or set LLVM_CONFIG.")
endif()

# Get necessary flags from llvm-config
execute_process(
    COMMAND ${LLVM_CONFIG} --cxxflags
    OUTPUT_VARIABLE LLVM_CXX_FLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND ${LLVM_CONFIG} --ldflags
    OUTPUT_VARIABLE LLVM_LD_FLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND ${LLVM_CONFIG} --libs core orcjit support mc x86
    OUTPUT_VARIABLE LLVM_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Apply compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_CXX_FLAGS}")

include_directories(header)

# Source files
add_executable(cat
  src/main.cpp
  src/lexer.cpp
  src/parser.cpp
  src/codegen.cpp
  src/ast.cpp
)

# Link against LLVM using the flags from llvm-config
target_link_libraries(cat PRIVATE ${LLVM_LD_FLAGS} ${LLVM_LIBS})

# Testing
enable_testing()

add_test(
  NAME RunTest
  COMMAND $<TARGET_FILE:cat>
)

set_tests_properties(RunTest PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
