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
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

#----------------------------------------------------------------------------------------------------------------------- 
# add project CMake module include path
#
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
include(module/Colors)


#----------------------------------------------------------------------------------------------------------------------- 
# set paths to the desired toolchains of corresponding OS
#
message("${Blue}Use GNU ARM toolchain GCC-9.3.1${ColourReset}")
set(WIN_TOOLCHAIN_PATH "D:/devtools/NXP/MCUXpressoIDE_11.7.1_9221/ide/plugins/com.nxp.mcuxpresso.tools.win32_11.7.1.202301190959/tools" CACHE PATH "Windows toolchain for GNU ARM") 
set(UNIX_TOOLCHAIN_PATH "/Applications/ARM/gcc-arm-none-eabi-9-2019-q4-major" CACHE PATH "Unix toolchain for GNU ARM") 

#----------------------------------------------------------------------------------------------------------------------- 
# check path and deside toolchain should be taken
#
set(COMPILER_PREFIX "arm-none-eabi")

# toolchain provided by windows 
if (EXISTS ${WIN_TOOLCHAIN_PATH})
    #message("${Yellow}Use toolchain provided by windows: ${WIN_TOOLCHAIN_PATH}${ColourReset}")
    set(TOOLCHAIN_PATH ${WIN_TOOLCHAIN_PATH})
    set(TOOLCHAIN_FILE_EXTENSION ".exe" CACHE INTERNAL "")

# toolchain provided by unix
elseif (EXISTS ${UNIX_TOOLCHAIN_PATH})
    set(TOOLCHAIN_PATH ${UNIX_TOOLCHAIN_PATH})
    #message("${Green}Use toolchain provided by unix: ${UNIX_TOOLCHAIN_PATH}${ColourReset}")

# none of the toolchains has been found
else ()
    message(FATAL_ERROR "${Red}None of provided toolchains exist, update path to the desired compiler within provided toolchain.cmake${ColourReset}")
endif ()


# todo NOT SURE WE NEED THIS
# here is the target environment located 
set(CMAKE_FIND_ROOT_PATH  ${TOOLCHAIN_PATH}/bin ${TOOLCHAIN_PATH})


#----------------------------------------------------------------------------------------------------------------------- 
# define compiler and tools for further usage
#

# define executebals for CMAKE compilers
set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/bin/${COMPILER_PREFIX}-gcc${TOOLCHAIN_FILE_EXTENSION} CACHE FILEPATH "File path to ASM Compiler")
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/bin/${COMPILER_PREFIX}-g++${TOOLCHAIN_FILE_EXTENSION} CACHE FILEPATH "File path to C Compiler")
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PATH}/bin/${COMPILER_PREFIX}-gcc${TOOLCHAIN_FILE_EXTENSION} CACHE FILEPATH "File path to C++ Compiler")
set(CMAKE_ASM_FLAGS -c)

# define executebals for USER commands
set(CMAKE_SIZE ${TOOLCHAIN_PATH}/bin/${COMPILER_PREFIX}-size${TOOLCHAIN_FILE_EXTENSION} CACHE FILEPATH "File path to Size tool of Compiler ")

# Variables initialized first time
set(CMAKE_CXX_FLAGS_INIT "-std=c++11")
set(CMAKE_C_FLAGS_INIT   "-std=gnu99")

# use static lib so not compiler test programs are executed
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
