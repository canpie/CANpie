#----------------------------------------------------------------------------------------------------------------------#
# This file contains toolchain specific defines and paths                                                              #
#                                                                                                                      #
# The implementation is oriented towards:                                                                              #
# https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/CrossCompiling                                          #
#                                                                                                                      #
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
message("${Blue}-> Use macOS (Darwin) XCode toolchain ${ColourReset}")


#----------------------------------------------------------------------------------------------------------------------- 
# check path and deside toolchain should be taken
#
set(COMPILER_PREFIX "")
set(TOOLCHAIN_PATH "/usr/bin")


#----------------------------------------------------------------------------------------------------------------------- 
# define compiler and tools for further usage
#
set(CMAKE_C_COMPILER   ${TOOLCHAIN_PATH}/clang   CACHE PATH "")
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/clang++ CACHE PATH "")
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PATH}/clang   CACHE PATH "")


#----------------------------------------------------------------------------------------------------------------------- 
# Variables initialized first time
#
set(CMAKE_CXX_FLAGS_INIT "-std=c++17")
set(CMAKE_C_FLAGS_INIT   "-std=gnu99")

# use static lib so not compiler test programs are executed
# set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
