#----------------------------------------------------------------------------------------------------------------------#
# CMake file for testing QCan classes                                                                                  #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

#-------------------------------------------------------------------------------------------------------
# add project CMake module include path
#
SET( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake" ${CMAKE_MODULE_PATH})
INCLUDE(CpDirectories)

message("-- Configure test cases for QCan classes")


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

#-------------------------------------------------------------------------------------------------------
# Use C++ 17 standard
#
set(CMAKE_CXX_STANDARD 17)

#-------------------------------------------------------------------------------------------------------
# Configure Qt support
# Do not use the 'set(CMAKE_AUTOMOC ON)' command here, because this will produce conflicting header
# files which are automatically included.
#
set(QT_VERSION_MAJOR 0)

if (${QT_VERSION_MAJOR} EQUAL 0)
   find_package(Qt5 QUIET COMPONENTS Core Network WebSockets Test )
   if (Qt5_FOUND)
      set(QT_VERSION_MAJOR 5)
      message("-- Found Qt version 5" )
   endif()
endif()


if (${QT_VERSION_MAJOR} EQUAL 0)
   find_package(Qt6 QUIET COMPONENTS Core Network WebSockets Test )
   if (Qt6_FOUND)
      set(QT_VERSION_MAJOR 6)
      message("-- Found Qt version 6" )
   endif()
endif()


if (${QT_VERSION_MAJOR} EQUAL 0)
   message(FATAL_ERROR  "-- No matchig Qt version (5 or 6) found")
endif()


set(CMAKE_AUTOMOC ON)

#-------------------------------------------------------------------------------------------------------
# Disable Qt debug output when not using the Debug build type
#
if(NOT CMAKE_BUILD_TYPE MATCHES Debug)
   add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()


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
target_link_libraries(${PROJECT_NAME} 
                      Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network 
                      Qt${QT_VERSION_MAJOR}::WebSockets Qt${QT_VERSION_MAJOR}::Test)

#-------------------------------------------------------------------------------------------------------
# copy program to bin directory 
#
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E echo "Copy ${PROJECT_NAME} to ${CP_PATH_BIN}"
                   COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_NAME} ${CP_PATH_BIN})
