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
project(canpie-fd-tools )


message(STATUS "Enter source/application directory ..")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/can-config)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/can-dump)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/can-error)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/can-send)

if (NOT CMAKE_HOST_SYSTEM_PROCESSOR MATCHES armv7l)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/server)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/plugins)
endif()