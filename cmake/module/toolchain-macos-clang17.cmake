#----------------------------------------------------------------------------------------------------------------------#
# This file contains toolchain specific defines and paths                                                              #
#                                                                                                                      #
# The implementation is oriented towards:                                                                              #
# https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/CrossCompiling                                          #
#                                                                                                                      #
# Make sure the cmake is triggered with cmake -G"MinGW Makefiles"                                                      #
#----------------------------------------------------------------------------------------------------------------------#

#----------------------------------------------------------------------------------------------------------------------- 
# target system definition
#
set(CMAKE_SYSTEM_NAME      Darwin)
set(CMAKE_SYSTEM_PROCESSOR arm64)

#----------------------------------------------------------------------------------------------------------------------- 
# add project CMake module include path
#
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
include(module/Colors)


#----------------------------------------------------------------------------------------------------------------------- 
# set paths to the desired toolchains of corresponding OS
#
message("${Blue}-> Use macOS (Darwin) clang 17 toolchain ${ColourReset}")


#----------------------------------------------------------------------------------------------------------------------- 
# check path and deside toolchain should be taken
#
set(COMPILER_SUFFIX "-mp-17")
set(TOOLCHAIN_PATH "/opt/local/bin")


#----------------------------------------------------------------------------------------------------------------------- 
# define compiler and tools for further usage
#
set(CMAKE_C_COMPILER   ${TOOLCHAIN_PATH}/clang${COMPILER_SUFFIX}   CACHE PATH "")
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/clang++${COMPILER_SUFFIX} CACHE PATH "")
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PATH}/clang${COMPILER_SUFFIX}   CACHE PATH "")


#----------------------------------------------------------------------------------------------------------------------- 
# Variables initialized first time
#
set(CMAKE_CXX_FLAGS_INIT "-std=c++17")
set(CMAKE_C_FLAGS_INIT   "-std=gnu99")

