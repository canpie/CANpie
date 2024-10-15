#----------------------------------------------------------------------------------------------------------------------#
# This file contains toolchain specific defines and paths                                                              #
#                                                                                                                      #
# The implementation is oriented towards:                                                                              #
# https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/CrossCompiling                                          #
#                                                                                                                      #
# Make sure the cmake is triggered with cmake -G"MinGW Makefiles" or -G"Unix Makefiles"                                #
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
Include(module/Colors)


#----------------------------------------------------------------------------------------------------------------------- 
# set paths to the desired toolchains of corresponding OS
#
message( CHECK_START  "${Blue}Check for toolchain GCC-10.2.1${ColourReset}")
set(WIN_TOOLCHAIN_PATH "D:/devtools/GNU_ARM/10 2020-q4-major" CACHE PATH "Windows toolchain for GNU ARM") 
set(UNIX_TOOLCHAIN_PATH "/Applications/ARM/gcc-arm-none-eabi-10-2020-q4-major" CACHE PATH "Unix toolchain for GNU ARM") 

#----------------------------------------------------------------------------------------------------------------------- 
# check path and deside toolchain should be taken
#
set(COMPILER_PREFIX "arm-none-eabi")

#----------------------------------------------------------------------------------------------------------------------- 
# check toolchain provided by windows 
#
if (WIN32)

   if (EXISTS ${WIN_TOOLCHAIN_PATH})
      message( CHECK_PASS "${Green}found ${WIN_TOOLCHAIN_PATH}${ColourReset}")
      
      set(TOOLCHAIN_PATH ${WIN_TOOLCHAIN_PATH} CACHE PATH "Toolchain path")
      set(TOOLCHAIN_FILE_EXTENSION ".exe" CACHE INTERNAL "")
   else()
      message( CHECK_FAIL "Toolchain does not exist, set WIN_TOOLCHAIN_PATH to a valid path.")
   endif()

#----------------------------------------------------------------------------------------------------------------------- 
# toolchain provided by unix
#
else()

   if (EXISTS ${UNIX_TOOLCHAIN_PATH})
      message( CHECK_PASS "${Green}found ${UNIX_TOOLCHAIN_PATH}${ColourReset}")
      set(TOOLCHAIN_PATH ${UNIX_TOOLCHAIN_PATH} CACHE PATH "Toolchain path")
   else()
      message( CHECK_FAIL "Toolchain does not exist, set UNIX_TOOLCHAIN_PATH to a valid path.")
   endif()

endif()

if (NOT TOOLCHAIN_PATH)
   message(FATAL_ERROR "The specified path to toolchain does not exist!")
endif()
#----------------------------------------------------------------------------------------------------------------------- 
# here is the target environment located 
#
set(CMAKE_FIND_ROOT_PATH  ${TOOLCHAIN_PATH}/bin ${TOOLCHAIN_PATH})


#----------------------------------------------------------------------------------------------------------------------- 
# define compiler and tools for further usage
#

# define executebals for CMAKE compilers
set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/bin/${COMPILER_PREFIX}-gcc${TOOLCHAIN_FILE_EXTENSION} CACHE PATH "")
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/bin/${COMPILER_PREFIX}-g++${TOOLCHAIN_FILE_EXTENSION} CACHE PATH "")
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PATH}/bin/${COMPILER_PREFIX}-gcc${TOOLCHAIN_FILE_EXTENSION} CACHE PATH "")
set(CMAKE_ASM_FLAGS -c)

# define executebals for USER commands
set(CMAKE_SIZE ${TOOLCHAIN_PATH}/bin/arm-none-eabi-size${TOOLCHAIN_FILE_EXTENSION} CACHE PATH "")

# Variables initialized first time
set(CMAKE_CXX_FLAGS_INIT "-std=c++11")
set(CMAKE_C_FLAGS_INIT   "-std=gnu99")

# use static lib so not compiler test programs are executed
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
