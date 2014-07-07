cmake_minimum_required(VERSION 2.8)
project(test)

# relase binary folder
set(CMAKE_RELEASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/release")
# debug binary folder
set(CMAKE_DEBUG_DIR "{CMAKE_CURRENT_SOURCE_DIR}/build/debug")

# set compiler for using of default gcc via macport (gcc 4.8)
#set(CMAKE_CXX_COMPILER "/opt/local/bin/g++")
#set(CMAKE_C_COMPILER "/opt/local/bin/gcc")

# set the release mode for build type
set(CMAKE_BUILD_TYPE "Release")

# set c++ compiler flags -on warning and using of new c++ strandard library ISO 2011 (-std=c++0x)
set(CMAKE_CXX_FLAGS "-Wall -std=c++11")

find_package(OpenCV REQUIRED)

# message for set the compiler and its flags and type of build(release)
message ("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
message ("CMAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS}")
message ("CMAKE_CXX_FLAGS_RELEASE is ${CMAKE_CXX_FLAGS_RELEASE}")

add_executable(calibration camera_calibration.cpp)
target_link_libraries (calibration ${OpenCV_LIBS})


if(CMAKE_BUILD_TYPE MATCHES DEBUG OR CMAKE_BUILD_TYPE MATCHES "Debug")
	set_target_properties(calibration PROPERTIES DEBUG_PREFIX "_d")
	set_target_properties(calibration PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_DEBUG_DIR})
else()
	set_target_properties(calibration PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RELEASE_DIR})
endif()



