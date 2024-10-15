#----------------------------------------------------------------------------------------------------------------------#
# CMake file for testing CANpie and NRL files                                                                          #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

#-------------------------------------------------------------------------------------------------------
# specify minimum version of CMake that can be used with CMakeLists.txt file
#
cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

#-------------------------------------------------------------------------------------------------------
# This project uses a CMakeLists.txt and can be translated with the following commands:
# > mkdir build
# > cd build
# > cmake ..
# > cmake --build .
# 


#-------------------------------------------------------------------------------------------------------
# specify project
#
project("CANpie FD")


#-------------------------------------------------------------------------------------------------------
# Check if directory 'fff' is present
#
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/fff/fff.h")
   message("   :: Header file for Fake Function Framework found")
   set(FFF_IS_INSTALLED  1)
else()
   message("   :: Header file for Fake Function Framework missing - tests not generated")
   set(FFF_IS_INSTALLED  0)
endif()

if(${FFF_IS_INSTALLED} EQUAL 0)
   message(STATUS "-------------------------------------------------------------------------")
   message(STATUS "Please check the README.md file in the directory"                         )
   message(STATUS " ${CMAKE_CURRENT_SOURCE_DIR} "                                            )
   message(STATUS "for instructions how to get the required header file."                    )
   message(STATUS "-------------------------------------------------------------------------")
endif()


#-------------------------------------------------------------------------------------------------------
# run tests
#
if(${FFF_IS_INSTALLED} EQUAL 1)
   
   message("-- Enter test directory ..")

   enable_testing()
   add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/qcan)
   add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test-canpie-core)

endif()
