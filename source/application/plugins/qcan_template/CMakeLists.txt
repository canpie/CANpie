#----------------------------------------------------------------------------------------------------------------------#
# CMake file plugin QCanTemplate                                                                                       #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.16.4 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

message("-- Configure plugin QCanTemplate")

#-------------------------------------------------------------------------------------------------------
# define the project name and version
#
project(can-send VERSION 1.00.4)


#-------------------------------------------------------------------------------------------------------
# add project CMake module include path
#
SET( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake" ${CMAKE_MODULE_PATH})
INCLUDE(AddCompilerFlags )
INCLUDE(AddSourceDirectories )



#-------------------------------------------------------------------------------------------------------
# specify include paths 
#
include_directories(${CP_PATH_MISC})
include_directories(${CP_PATH_QCAN})

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
   qcan_interface_template.cpp
   qcan_plugin_template.cpp
)

list(
   APPEND QCAN_SOURCES
   ${CP_PATH_QCAN}/qcan_frame.cpp
   ${CP_PATH_QCAN}/qcan_timestamp.cpp
)



set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

#-------------------------------------------------------------------------------------------------------
# create library from selected source files
#
add_library(${PROJECT_NAME} ${PLUGIN_SOURCES} ${QCAN_SOURCES})

#-------------------------------------------------------------------------------------------------------
# link Qt libs 
#
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Widgets)

#-------------------------------------------------------------------------------------------------------
# copy program to bin directory 
#
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#                   COMMAND ${CMAKE_COMMAND} -E echo "       Copy ${PROJECT_NAME} to ${CP_PATH_BIN}"
#                   COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_NAME} ${CP_PATH_BIN})
