cmake_minimum_required(VERSION 3.10)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the vcpkg toolchain file
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")

# Project name
project(miningsim)

# Add Packages
find_package(Boost REQUIRED COMPONENTS algorithm)

# Get all the source files
file(GLOB SOURCES "src/*.cpp")

# Add the executable
add_executable(miningsim ${SOURCES})

# Include directories
include_directories(miningsim PRIVATE ${Boost_INCLUDE_DIRS})

# Link libraries
target_link_libraries(miningsim PUBLIC 
                      Boost::boost
                      Boost::algorithm
                      ws2_32)