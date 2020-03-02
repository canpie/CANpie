cmake_minimum_required(VERSION 3.16.4)

message("Test cases for QCan classes")


#-----------------------------------------------------------------------------------------------------------------------
# define the project name and target file
#
project(qcantest VERSION 0.99.02)


#-----------------------------------------------------------------------------------------------------------------------
# set general paths of the project
#
set(TEST_PATH   ${CMAKE_CURRENT_SOURCE_DIR})
set(PROG_PATH   ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../source)
set(CANPIE_PATH ${SOURCE_PATH}/canpie-fd)
set(DEVICE_PATH ${SOURCE_PATH}/device/qcan)
set(QCAN_PATH   ${SOURCE_PATH}/qcan)

#-----------------------------------------------------------------------------------------------------------------------
# specify include paths 
#
include_directories(${TEST_PATH})
include_directories(${CANPIE_PATH})
include_directories(${DEVICE_PATH})
include_directories(${QCAN_PATH})
include_directories(${SOURCE_PATH}/misc)

#----------------------------------------------------------------------------------------------------------------------- 
# define source files for compilation
#
list(
    APPEND test_sources
    ${TEST_PATH}/test_main.cpp
    ${TEST_PATH}/test_qcan_filter.cpp
    ${TEST_PATH}/test_qcan_frame.cpp
    ${TEST_PATH}/test_qcan_socket.cpp
    ${TEST_PATH}/test_qcan_timestamp.cpp
)

list(
    APPEND qcan_sources
    ${QCAN_PATH}/qcan_filter.cpp
    ${QCAN_PATH}/qcan_filter_list.cpp
    ${QCAN_PATH}/qcan_frame.cpp
    ${QCAN_PATH}/qcan_socket.cpp
    ${QCAN_PATH}/qcan_timestamp.cpp
)

list(
    APPEND canpie_sources
    ${CANPIE_PATH}/cp_msg.c
)

list(
    APPEND device_sources
    ${DEVICE_PATH}/qcan_socket_canpie_fd.cpp
)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core Network Websockets Test REQUIRED)


#----------------------------------------------------------------------------------------------------------------------- 
# create binary from selected source files
#
add_executable(${PROJECT_NAME} 
    ${test_sources}
    ${qcan_sources}
    ${canpie_sources}
    ${device_sources}
)

#-----------------------------------------------------------------------------------------------------------------------
# link Qt libs 
#
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network Qt5::WebSockets Qt5::Test)

#-----------------------------------------------------------------------------------------------------------------------
# copy program to bin directory 
#
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E echo "Copy ${PROJECT_NAME} to ${PROG_PATH}"
                   COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_NAME} ${PROG_PATH})
