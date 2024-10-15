#----------------------------------------------------------------------------------------------------------------------#
# CMake file for testing classes                                                                                  #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.16.3 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

#-------------------------------------------------------------------------------------------------------
# add project CMake module include path
#
SET( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake" ${CMAKE_MODULE_PATH})
INCLUDE(CpDirectories)
INCLUDE(AddSplint)


message("-- Configure test cases for CANpie FD header files and sources")


#-------------------------------------------------------------------------------------------------------
# prepare for testing
#
enable_testing()


#-------------------------------------------------------------------------------------------------------
# define the project name and target file
#
project(canpie-fd VERSION 0.99.02)

add_definitions(-D CP_NRL_CHANNEL_MAX=1)

#-------------------------------------------------------------------------------------------------------
# specify include paths 
#
include_directories(${CP_PATH_CANPIE})
include_directories(${CP_PATH_MISC})
include_directories(${CP_PATH_DEVICE_QCAN})


#----------------------------------------------------------------------------------------------------------------------- 
# define source files for compilation
#

list(
   APPEND function_sources
   test_cp_core.c
   test_cp_main_f.c
   test_cp_msg_ccf.c
   test_cp_msg_fdf.c
   unity_fixture.c
   unity.c
)

list(
   APPEND macro_sources
   test_cp_main_m.c
   test_cp_msg_ccm.c
   test_cp_msg_fdm.c
   unity_fixture.c
   unity.c
)

add_splint(canpie_test ${CP_FILE_CANPIE})

#----------------------------------------------------------------------------------------------------------------------- 
# create binary from selected source files
#
add_executable(canpie_test_function 
    ${CP_FILE_CANPIE}
    ${function_sources}
)

add_executable(canpie_test_macro
    ${CP_FILE_CANPIE}
    ${macro_sources}
)


#------------------------------------------------------------------------------
# add some test
#
add_test(canpie_test_function canpie_test_function)
add_test(canpie_test_macro canpie_test_macro)