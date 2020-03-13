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

message("Test cases for QCan classes")


#-------------------------------------------------------------------------------------------------------
# define the project name and target file
#
project(qcantest VERSION 0.99.02)


#-------------------------------------------------------------------------------------------------------
# specify include paths 
#
include_directories(${CP_PATH_CANPIE})
include_directories(${CP_PATH_DEVICE_QCAN})
include_directories(${CP_PATH_MISC})
include_directories(${CP_PATH_QCAN})

#------------------------------------------------------------------------------------------------------- 
# define source files for compilation
#
list(
    APPEND TEST_SOURCES
    test_main.cpp
    test_qcan_filter.cpp
    test_qcan_frame.cpp
    test_qcan_socket.cpp
    test_qcan_socket_canpie.cpp
    test_qcan_timestamp.cpp
)

list(
    APPEND QCAN_SOURCES
    ${CP_PATH_QCAN}/qcan_filter.cpp
    ${CP_PATH_QCAN}/qcan_filter_list.cpp
    ${CP_PATH_QCAN}/qcan_frame.cpp
    ${CP_PATH_QCAN}/qcan_socket.cpp
    ${CP_PATH_QCAN}/qcan_timestamp.cpp
)



set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core Network WebSockets Test REQUIRED)


#-------------------------------------------------------------------------------------------------------
# create binary from selected source files
#
add_executable(${PROJECT_NAME} 
    ${TEST_SOURCES}
    ${QCAN_SOURCES}
    ${CP_FILE_CANPIE}
    ${CP_FILE_DEVICE_QCAN}
)

#-------------------------------------------------------------------------------------------------------
# link Qt libs 
#
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network Qt5::WebSockets Qt5::Test)

#-------------------------------------------------------------------------------------------------------
# copy program to bin directory 
#
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E echo "Copy ${PROJECT_NAME} to ${CP_PATH_BIN}"
                   COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_NAME} ${CP_PATH_BIN})
