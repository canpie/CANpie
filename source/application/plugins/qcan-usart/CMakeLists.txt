#----------------------------------------------------------------------------------------------------------------------#
# CMake file plugin QCanUsart                                                                                          #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

message("-- Configure plugin QCanUsart")

#-------------------------------------------------------------------------------------------------------
# define the project name and version
#
project(QCanUsart VERSION 1.02.2)


#-------------------------------------------------------------------------------------------------------
# add project CMake module include path
#
SET( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake" ${CMAKE_MODULE_PATH})
INCLUDE(module/CompilerFlags)
INCLUDE(CpDirectories)


#-------------------------------------------------------------------------------------------------------
# Make sure no sub-directories are automatically created for the binary during build
#
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_CURRENT_BINARY_DIR})


#-------------------------------------------------------------------------------------------------------
# specify include paths 
#
include_directories(${CP_PATH_CANPIE})
include_directories(${CP_PATH_MISC})
include_directories(${CP_PATH_QCAN})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/forms)

#-------------------------------------------------------------------------------------------------------
# pass version information to application
#
add_definitions(-DVERSION_MAJOR=${PROJECT_VERSION_MAJOR})
add_definitions(-DVERSION_MINOR=${PROJECT_VERSION_MINOR})
add_definitions(-DVERSION_BUILD=${PROJECT_VERSION_PATCH})


#------------------------------------------------------------------------------------------------------- 
# define source files for compilation
#
list(
   APPEND PLUGIN_SOURCES
   ./forms/qcan_config.cpp
   cp_usart.c
   qcan_interface_usart.cpp
   qcan_plugin_usart.cpp
   qcan_usart.cpp
)

list(
   APPEND QCAN_SOURCES
   ${CP_PATH_QCAN}/qcan_frame.cpp
   ${CP_PATH_QCAN}/qcan_plugin.cpp
   ${CP_PATH_QCAN}/qcan_timestamp.cpp
)

list (APPEND RESOURCES qcan_usart.qrc)

#-------------------------------------------------------------------------------------------------------
# Test for Linux system 
#
if(UNIX AND NOT APPLE)
   set(LINUX TRUE)
endif()


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
   find_package(Qt5 QUIET COMPONENTS Core SerialPort Widgets )
   if (Qt5_FOUND)
      set(QT_VERSION_MAJOR 5)
      message("-- Found Qt version 5" )
   endif()
endif()


if (${QT_VERSION_MAJOR} EQUAL 0)
   find_package(Qt6 QUIET COMPONENTS Core SerialPort Widgets )
   if (Qt6_FOUND)
      set(QT_VERSION_MAJOR 6)
      message("-- Found Qt version 6" )
   endif()
endif()


if (${QT_VERSION_MAJOR} EQUAL 0)
   message(FATAL_ERROR  "-- No matchig Qt version (5 or 6) found")
endif()


#-------------------------------------------------------------------------------------------------------
# Qt specific setup: use moc, rcc and uic, check for required Qt libraries
#
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ./forms)

#-------------------------------------------------------------------------------------------------------
# definition for number of channels
#
add_definitions(-DMC_USART_PORT_MAX=CP_CHANNEL_MAX)


#-------------------------------------------------------------------------------------------------------
# create plugin from selected source files
#
add_library(${PROJECT_NAME} SHARED ${PLUGIN_SOURCES} ${QCAN_SOURCES} ${CP_FILE_CANPIE} ${RESOURCES})


#-------------------------------------------------------------------------------------------------------
# link Qt libs 
#
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::SerialPort Qt${QT_VERSION_MAJOR}::Widgets)


#-------------------------------------------------------------------------------------------------------
# copy library to plugins directory:
#
set(LIB_NAME ${CMAKE_SHARED_LIBRARY_PREFIX}${PROJECT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E echo "       Copy ${LIB_NAME} to ${CP_PATH_PLUGINS}/"
                   COMMAND ${CMAKE_COMMAND} -E copy ${LIB_NAME} ${CP_PATH_PLUGINS}/)
