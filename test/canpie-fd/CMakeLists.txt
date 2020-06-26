#----------------------------------------------------------------------------------------------------------------------#
# CMake file for testing QCan classes                                                                                  #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.16.4 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

#-------------------------------------------------------------------------------------------------------
# add project CMake module include path
#
SET( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake" ${CMAKE_MODULE_PATH})
INCLUDE(AddSourceDirectories )
INCLUDE(AddSplint )

message("-- Configure test cases for CANpie FD header files and sources")


#-------------------------------------------------------------------------------------------------------
# prepare for testing
#
enable_testing()


#-------------------------------------------------------------------------------------------------------
# define the project name and target file
#
project(canpie-fd VERSION 0.99.02)


#-------------------------------------------------------------------------------------------------------
# specify include paths 
#
include_directories(${CP_PATH_CANPIE})
include_directories(${CP_PATH_DEVICE_TEMPLATE})
include_directories(${CP_PATH_MISC})
include_directories(${CP_PATH_QCAN})

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

add_splint(canpie_test ${CP_FILE_CANPIE} ${CP_FILE_DEVICE_TEMPLATE})

#----------------------------------------------------------------------------------------------------------------------- 
# create binary from selected source files
#
add_executable(canpie_test_function 
    ${CP_FILE_CANPIE}
    ${CP_FILE_DEVICE_TEMPLATE}
    ${function_sources}
)

add_executable(canpie_test_macro
    ${CP_FILE_CANPIE}
    ${CP_FILE_DEVICE_TEMPLATE}
    ${macro_sources}
)

add_test( cp_func canpie_test_function )


#-------------------------------------------------------------------------------------------------------
# copy program to bin directory 
#
add_custom_command(TARGET canpie_test_function POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E echo "Copy canpie_test_function to ${CP_PATH_BIN}"
                   COMMAND ${CMAKE_COMMAND} -E copy canpie_test_function ${CP_PATH_BIN})

add_custom_command(TARGET canpie_test_macro POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E echo "Copy canpie_test_macro to ${CP_PATH_BIN}"
                   COMMAND ${CMAKE_COMMAND} -E copy canpie_test_macro ${CP_PATH_BIN})