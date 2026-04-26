cmake_minimum_required(VERSION 3.10)
project(MyUGen)

# Force user to provide the SuperCollider source path
if (NOT SC_PATH)
    message(FATAL_ERROR "Please specify SC_PATH (e.g., cmake -DSC_PATH=/path/to/supercollider ..)")
endif()

# SuperCollider modern plugins usually use C++14 or C++17
set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Optimization flags: Crucial for audio DSP on ARM processors
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -mcpu=native -mtune=native -ffast-math")

# Create the shared library module
add_library(${PROJECT_NAME} MODULE MyUGen.cpp)

# SuperCollider expects Linux plugins to end in .so without a "lib" prefix
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".so")

# Include the necessary SuperCollider headers
target_include_directories(${PROJECT_NAME} PRIVATE
    ${SC_PATH}/include/plugin_interface
    ${SC_PATH}/include/common
    ${SC_PATH}/external_libraries/nova-simd
)