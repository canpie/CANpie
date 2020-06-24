#----------------------------------------------------------------------------------------------------------------------#
# CMake file plugin QCanUsart                                                                                          #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.16.4 FATAL_ERROR)
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
INCLUDE(AddCompilerFlags )
INCLUDE(AddSourceDirectories )


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
   ${CP_PATH_QCAN}/qcan_timestamp.cpp
)

list (APPEND RESOURCES qcan_usart.qrc)


#-------------------------------------------------------------------------------------------------------
# Qt specific setup: use moc, rcc and uic, check for required Qt libraries
#
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ./forms)

find_package(Qt5 COMPONENTS Core SerialPort Widgets REQUIRED)

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
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::SerialPort Qt5::Widgets)


#-------------------------------------------------------------------------------------------------------
# copy library to plugins directory:
#
set(LIB_NAME ${CMAKE_SHARED_LIBRARY_PREFIX}${PROJECT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E echo "       Copy ${LIB_NAME} to ${CP_PATH_PLUGINS}/"
                   COMMAND ${CMAKE_COMMAND} -E copy ${LIB_NAME} ${CP_PATH_PLUGINS}/)
