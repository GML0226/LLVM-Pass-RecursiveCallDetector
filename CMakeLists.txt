cmake_minimum_required(VERSION 3.20)
project(RecursiveCallDetectorPass LANGUAGES CXX)

# Find LLVM package, specify version 19.1 to match 19.1.7
find_package(LLVM 19.1 CONFIG NO_DEFAULT_PATH PATHS /usr/lib/llvm-19/lib/cmake/llvm)
if(NOT LLVM_FOUND)
  message(FATAL_ERROR "LLVM 19.1 not found. Ensure LLVM 19 is installed and LLVM_DIR points to /usr/lib/llvm-19/lib/cmake/llvm.")
endif()

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Include LLVM directories and definitions
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS)
add_definitions(${LLVM_DEFINITIONS})

# Set C++ standard to match LLVM
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set optimization flags for Release build
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

# Build shared library for the Pass
add_library(RecursiveCallDetector SHARED RecursiveCallDetector.cpp)

# Link against LLVM libraries
target_link_libraries(RecursiveCallDetector PRIVATE ${LLVM_LIBRARIES})

# Set output directory for the shared library
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)