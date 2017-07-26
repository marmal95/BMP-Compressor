cmake_minimum_required(VERSION 3.7)

# Solution Name
project(BMP-Compressor)

# C++ Version
set(CMAKE_CXX_STANDARD 14)

# Set CMake modules path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# Set SDL2 root path
# set(SDL2_PATH "")

# Set SDL2 Path
if (NOT SDL2_PATH)
    SET(SDL2_PATH "" CACHE STRING "Path to SDL2 root.")
endif ()

# Find SDL2
find_package(SDL2 REQUIRED)
IF(SDL2_FOUND)
    include_directories(${SDL2_INCLUDE_DIR})
endif()

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# Include Directories
include_directories(include)

# Add files
file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "source/*.cpp")

# Create named folders for the sources within the .vcproj
# Empty name lists them directly under the .vcproj
source_group("include" FILES ${HEADERS})
source_group("source" FILES ${SOURCES})

# Create .exe
add_executable(BMP-Compressor ${HEADERS} ${SOURCES})

# Link libraries
target_link_libraries(BMP-Compressor ${SDL2_LIBRARY})