#----------------------------------------------------------------------------------------------------------------------#
# CMake file for testing QCan classes                                                                                  #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

#-------------------------------------------------------------------------------------------------------
# specify minimum version of CMake that can be used with CMakeLists.txt file
#
cmake_minimum_required(VERSION 3.16.4 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

#------------------------------------------------------------------------------
# This project consists of a CMakeLists.txt and can be translated with the 
# following commands:
# > mkdir build
# > cd build
# > cmake ..
# > cmake --build .
# 

message("-- Enter test directory ..")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/canpie-fd)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/qcan)

