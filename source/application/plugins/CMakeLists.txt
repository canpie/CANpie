#----------------------------------------------------------------------------------------------------------------------#
# CMake file for building CANpie FD server abd tools                                                                   #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

#-------------------------------------------------------------------------------------------------------
# specify minimum version of CMake that can be used with CMakeLists.txt file
#
cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)


#-------------------------------------------------------------------------------------------------------
# define the project name
#
project(canpie-server-plugins )

message("-- Configure plugins ..")


#-------------------------------------------------------------------------------------------------------
# PCAN plugin only supported on Windows and macOS
#
if(WIN32 OR APPLE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/qcan-peak)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/qcan-template)

#-------------------------------------------------------------------------------------------------------
# IXXAT plugin only supported on Windows
#
if(WIN32)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/qcan-hms-vci4)
endif()

