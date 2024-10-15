#----------------------------------------------------------------------------------------------------------------------#
# CMake Superbuild configuration file for MCL                                                                          #
#                                                                                                                      #
# This superbuild configurateion file builds differente projects depending on following variables:                     #
# - BUILD_TARGET:FILEPATH is set and provided by user                                                                  # 
# - CMAKE_CROSSCOMPILING:BOOL set by CMake to true when user provides a toolchain file                                 # 
#                                                                                                                      #
# Depending on configuration of this both variables different projects                                                 #
# - If BUILD_TARGET is provided, only that project will be built, regardless of the compiler or toolchain.             #
# - If BUILD_TARGET is NOT provided, the predefine template project and tests will be built.                           #
#   This is also done regardless of the compiler or toolchain.                                                         #
# - If CMAKE_CROSSCOMPILING is set to true, then all board projects are additionally built.                            #
#                                                                                                                      #  
#----------------------------------------------------------------------------------------------------------------------#
cmake_minimum_required(VERSION 3.16.4 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

#-----------------------------------------------------------------------------------------------------------------------
# add project CMake module include path
#
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(module/Colors)


#-----------------------------------------------------------------------------------------------------------------------
#
#
message( NOTICE "Superbuild configuration for CANpie-FD sources" )
 

#-----------------------------------------------------------------------------------------------------------------------
# specify project
#
project(MCL)


#-----------------------------------------------------------------------------------------------------------------------
# This CMake can be used to compile tests or only project target provided by BUILD_TARGET define.
# The BUILD_TARGET defines are also managed by ./vscode/cmake-variants.json
#
# Make sure build folder is empty before switching, so delete it when switchting to ensure clean build!
#
message( CHECK_START "Check 'BUILD_TARGET'" )

# If a BUILD_TARGET is provided, the user is smart enough to choose the correct toolchain or system compiler, 
# so we don't worry about it here.
if(DEFINED BUILD_TARGET)
   message( CHECK_PASS "has been provided ${BUILD_TARGET}" )

   # The specified target will be translated regardless of the selected toolchain or compiler.
   add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}${BUILD_TARGET})

# Handle the situation when the BUILD_TARGET is NOT provided
else()
   message( CHECK_FAIL "has been not provided" )

   #-----------------------------------------------------------------------------------------------------------------
   # Build "Template for CANpie FD device" and all available "MCL tests"
   # 
   # \todo adjust this entries depending on available projects
   add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/application)
   add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

   #-----------------------------------------------------------------------------------------------------------------
   # Test compiler - it must be a cross compiler for EVAL board testing
   #
   message( CHECK_START "Check 'CMAKE_CROSSCOMPILING'" )
   if(CMAKE_CROSSCOMPILING)
      message( CHECK_PASS "provided ${TOOLCHAIN_PATH}" )
      
      # A croscomplier is absolutely necessary for testing the board porojects
      add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/device)
   else()
      message( CHECK_FAIL "not provided" )
   endif()

endif()


#-----------------------------------------------------------------------------------------------------------------------
# enable testing so sub-tests can be triggered by 'ctest' command
#
enable_testing()
