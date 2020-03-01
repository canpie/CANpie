cmake_minimum_required(VERSION 3.16.4)

message("Test cases for CANpie FD header files and sources")

SET( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake" ${CMAKE_MODULE_PATH})
INCLUDE(AddSplint )

#-----------------------------------------------------------------------------------------------------------------------
# prepare for testing
#
enable_testing()


#-----------------------------------------------------------------------------------------------------------------------
# define the project name and target file
#
project(canpie-fd VERSION 0.99.02)


#-----------------------------------------------------------------------------------------------------------------------
# set general paths of the project
#
set(TEST_PATH   ${CMAKE_CURRENT_SOURCE_DIR})
set(PROG_PATH   ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../source)
set(CANPIE_PATH ${SOURCE_PATH}/canpie-fd)
set(DEVICE_PATH ${SOURCE_PATH}/device/template)


#-----------------------------------------------------------------------------------------------------------------------
# specify include paths 
#
include_directories(${TEST_PATH})
include_directories(${CANPIE_PATH})
include_directories(${DEVICE_PATH})
include_directories(${SOURCE_PATH}/misc)

#----------------------------------------------------------------------------------------------------------------------- 
# define source files for compilation
#
list(
    APPEND canpie_sources
    ${CANPIE_PATH}/cp_msg.c
    ${CANPIE_PATH}/cp_fifo.c
)

list(
    APPEND device_sources
    ${DEVICE_PATH}/device_canfd.c
)

list(
    APPEND function_sources
    ${TEST_PATH}/test_cp_core.c
    ${TEST_PATH}/test_cp_main_f.c
	${TEST_PATH}/test_cp_msg_ccf.c
	${TEST_PATH}/test_cp_msg_fdf.c
	${TEST_PATH}/unity_fixture.c
	${TEST_PATH}/unity.c
)

list(
    APPEND macro_sources
    ${TEST_PATH}/test_cp_main_m.c
	${TEST_PATH}/test_cp_msg_ccm.c
	${TEST_PATH}/test_cp_msg_fdm.c
	${TEST_PATH}/unity_fixture.c
	${TEST_PATH}/unity.c
)

add_splint(cp_test_function ${canpie_sources} ${device_sources})

#----------------------------------------------------------------------------------------------------------------------- 
# create binary from selected source files
#
add_executable(canpie_test_function 
    ${canpie_sources}
    ${device_sources}
    ${function_sources}
)

add_executable(canpie_test_macro
    ${canpie_sources}
    ${device_sources}
    ${macro_sources}
)

add_test( cp_func canpie_test_function )

#-----------------------------------------------------------------------------------------------------------------------
# copy program to bin directory 
#
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#                   COMMAND ${CMAKE_COMMAND} -E echo "Copy ${PROJECT_NAME} to ${PROG_PATH}"
#                   COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_NAME} ${PROG_PATH})
