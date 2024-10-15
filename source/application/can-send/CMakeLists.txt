#----------------------------------------------------------------------------------------------------------------------#
# CMake file for can-send                                                                                              #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

message("-- Configure can-send")

#-------------------------------------------------------------------------------------------------------
# define the project name and version
#
project(can-send VERSION 0.99.10)


#-------------------------------------------------------------------------------------------------------
# add project CMake module include path
#
set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake" ${CMAKE_MODULE_PATH})
include(module/CompilerFlags)
include(CpDirectories)


#-------------------------------------------------------------------------------------------------------
# Make sure no sub-directories are automatically created for the binary during build
#
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CP_PATH_BIN})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CP_PATH_BIN})


#-------------------------------------------------------------------------------------------------------
# Set a default build type if none was specified
#
set(default_build_type "Release")
 
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


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
   find_package(Qt5 QUIET COMPONENTS Core Network WebSockets)
   if (Qt5_FOUND)
      set(QT_VERSION_MAJOR 5)
      message("-- Found Qt version 5" )
   endif()
endif()


if (${QT_VERSION_MAJOR} EQUAL 0)
   find_package(Qt6 QUIET COMPONENTS Core Network WebSockets)
   if (Qt6_FOUND)
      set(QT_VERSION_MAJOR 6)
      message("-- Found Qt version 6" )
   endif()
endif()


if (${QT_VERSION_MAJOR} EQUAL 0)
   message(FATAL_ERROR  "-- No matchig Qt version (5 or 6) found")
endif()


#-------------------------------------------------------------------------------------------------------
# Configure Qt support
#
set(CMAKE_AUTOMOC ON)

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
# Disable Qt debug output when not using the Debug build type
#
if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
   add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()


#------------------------------------------------------------------------------------------------------- 
# define source files for compilation
#
list(
   APPEND APP_SOURCES
   qcan_send.cpp
)

list(
   APPEND QCAN_SOURCES
   ${CP_PATH_QCAN}/qcan_frame.cpp
   ${CP_PATH_QCAN}/qcan_network_settings.cpp
   ${CP_PATH_QCAN}/qcan_server_settings.cpp
   ${CP_PATH_QCAN}/qcan_socket.cpp
   ${CP_PATH_QCAN}/qcan_timestamp.cpp
)


#-------------------------------------------------------------------------------------------------------
# create binary from selected source files
#
add_executable(${PROJECT_NAME} 
    ${APP_SOURCES}
    ${QCAN_SOURCES}

)

#-------------------------------------------------------------------------------------------------------
# Link Qt libs 
#
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::WebSockets)
if(WIN32)
   set(BINARY_NAME ${PROJECT_NAME}.exe)

else()
   set(BINARY_NAME ${PROJECT_NAME})
endif()


#-------------------------------------------------------------------------------------------------------
# Installation for Unix systems 
#
if (UNIX)
   install(PROGRAMS ${CP_PATH_BIN}/${PROJECT_NAME} DESTINATION /usr/local/bin)
endif ()
