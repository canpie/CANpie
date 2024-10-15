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
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

#----------------------------------------------------------------------------------------------------------------------- 
# add project CMake module include path
#
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
include(module/Colors)


#----------------------------------------------------------------------------------------------------------------------- 
# set paths to the desired toolchains of corresponding OS
#
message("${Blue}Use GNU ARM Linux toolchain ${ColourReset}")
set(UNIX_TOOLCHAIN_PATH "/usr/bin" CACHE PATH "Unix toolchain for GNU ARM") 

#----------------------------------------------------------------------------------------------------------------------- 
# check path and deside toolchain should be taken
#
set(COMPILER_PREFIX "arm-linux-gnueabihf")

set(TOOLCHAIN_PATH ${UNIX_TOOLCHAIN_PATH})

# todo NOT SURE WE NEED THIS
# here is the target environment located 
#set(CMAKE_FIND_ROOT_PATH  ${TOOLCHAIN_PATH}/bin ${TOOLCHAIN_PATH})


#----------------------------------------------------------------------------------------------------------------------- 
# define compiler and tools for further usage
#

# define executebals for CMAKE compilers
set(CMAKE_C_COMPILER   ${TOOLCHAIN_PATH}/${COMPILER_PREFIX}-gcc CACHE PATH "")
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/${COMPILER_PREFIX}-g++ CACHE PATH "")
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PATH}/${COMPILER_PREFIX}-gcc CACHE PATH "")
set(CMAKE_ASM_FLAGS -c)


# Variables initialized first time
set(CMAKE_CXX_FLAGS_INIT "-std=c++11")
set(CMAKE_C_FLAGS_INIT   "-std=gnu99")

# use static lib so not compiler test programs are executed
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
