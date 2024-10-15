#----------------------------------------------------------------------------------------------------------------------#
# This file contains toolchain specific defines and paths                                                              #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

#----------------------------------------------------------------------------------------------------------------------- 
# target system definition
#
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

#-------------------------------------------------------------------------------------------------------
# add project CMake module include path
#
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
INCLUDE(module/Colors)


#----------------------------------------------------------------------------------------------------------------------- 
# set paths to the desired toolchains of corresponding OS
#
set(WIN_TOOLCHAIN_PATH "/d/devtools/GNU_ARM/8 2019-q3-update" CACHE PATH "Windows toolchain for GNU ARM") 
set(UNIX_TOOLCHAIN_PATH "~/Applications/Compiler/gcc-arm-none-eabi" CACHE PATH "Unix toolchain for GNU ARM") 

#----------------------------------------------------------------------------------------------------------------------- 
# check path and deside toolchain should be taken
#

# toolchain provided by windows 
if (EXISTS ${WIN_TOOLCHAIN_PATH})
    message("${Green}Use toolchain provided by windows: ${WIN_TOOLCHAIN_PATH}${ColourReset}")
    set(TOOLCHAIN_PATH ${WIN_TOOLCHAIN_PATH})
    set(TOOLCHAIN_FILE_EXTENSION ".exe" CACHE INTERNAL "")

# toolchain provided by unix
elseif (EXISTS ${UNIX_TOOLCHAIN_PATH})
    set(TOOLCHAIN_PATH ${UNIX_TOOLCHAIN_PATH})
    message("${Green}Use toolchain provided by unix: ${UNIX_TOOLCHAIN_PATH}${ColourReset}")

# none of the toolchains has been found
else ()
    message(FATAL_ERROR "${Red}None of provided toolchains exist, update path to the desired compiler within provided toolchain.cmake${ColourReset}")
endif ()


#----------------------------------------------------------------------------------------------------------------------- 
# define compiler and tolls for further usage
#

# define executebals for CMAKE compilers
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PATH}/bin/arm-none-eabi-gcc${TOOLCHAIN_FILE_EXTENSION}" CACHE INTERNAL "File path to ASM Compiler")
set(CMAKE_C_COMPILER "${TOOLCHAIN_PATH}/bin/arm-none-eabi-gcc${TOOLCHAIN_FILE_EXTENSION}" CACHE INTERNAL "File path to C Compiler")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PATH}/bin/arm-none-eabi-g++${TOOLCHAIN_FILE_EXTENSION}" CACHE INTERNAL "File path to C++ Compiler")
set(CMAKE_ASM_FLAGS -c)

# define executebals for USER commands
set(CMAKE_SIZE "${TOOLCHAIN_PATH}/bin/arm-none-eabi-size${TOOLCHAIN_FILE_EXTENSION}" CACHE INTERNAL "File path to Size tool of Compiler ")

set(CMAKE_VERBOSE_MAKEFILE on)

# Variables initialized first time
set(CMAKE_CXX_FLAGS_INIT "-std=c++11")
set(CMAKE_C_FLAGS_INIT   "-std=gnu99")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
