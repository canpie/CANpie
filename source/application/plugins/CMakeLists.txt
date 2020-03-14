#----------------------------------------------------------------------------------------------------------------------#
# CMake file for building CANpie FD server abd tools                                                                   #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

#-------------------------------------------------------------------------------------------------------
# specify minimum version of CMake that can be used with CMakeLists.txt file
#
cmake_minimum_required(VERSION 3.16.4 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)


message("-- Configure plugins ..")

#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/qcan-ixxat)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/qcan-peak)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/qcan-template)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/qcan-usart)

