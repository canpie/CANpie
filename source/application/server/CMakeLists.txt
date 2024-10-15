#----------------------------------------------------------------------------------------------------------------------#
# CMake file for CANpieServer                                                                                          #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

message("-- Configure CANpieServer")

#-------------------------------------------------------------------------------------------------------
# define the project name and version
#
project(CANpieServer VERSION 1.00.02)


#-------------------------------------------------------------------------------------------------------
# add project CMake module include path
#
SET( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake" ${CMAKE_MODULE_PATH})
INCLUDE(module/CompilerFlags)
INCLUDE(CpDirectories)


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
   set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)

   #-----------------------------------------------------------------------------------------------
   # Set the possible values of build type for cmake-gui
   #
   set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

#-------------------------------------------------------------------------------------------------------
# Disable Qt debug output when not using the Debug build type
#
if(NOT CMAKE_BUILD_TYPE MATCHES Debug)
   add_definitions(-DQT_NO_DEBUG_OUTPUT)
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
#
set(QT_VERSION_MAJOR 0)

if (${QT_VERSION_MAJOR} EQUAL 0)
   find_package(Qt5 QUIET COMPONENTS Core Gui Network WebSockets Widgets)
   if (Qt5_FOUND)
      set(QT_VERSION_MAJOR 5)
      message("-- Found Qt version 5" )
   endif()
endif()


if (${QT_VERSION_MAJOR} EQUAL 0)
   find_package(Qt6 QUIET COMPONENTS Core Gui Network WebSockets Widgets)
   if (Qt6_FOUND)
      set(QT_VERSION_MAJOR 6)
      message("-- Found Qt version 6" )
   endif()
endif()


if (${QT_VERSION_MAJOR} EQUAL 0)
   message(FATAL_ERROR  "-- No matchig Qt version (5 or 6) found")
endif()


#-------------------------------------------------------------------------------------------------------
# Qt specific setup: use moc and rcc, check for required Qt libraries
#
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ./forms)


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
   APPEND APP_SOURCES
   qcan_interface_widget.cpp
   qcan_server_dialog.cpp
   server_main.cpp
)


#------------------------------------------------------------------------------------------------------- 
# macos: additional source files for compilation
#
if (APPLE)
   list(APPEND APP_SOURCES  MacAppNapSuspender.mm)
endif()

list(
   APPEND QCAN_SOURCES
   ${CP_PATH_QCAN}/qcan_frame.cpp
   ${CP_PATH_QCAN}/qcan_network.cpp
   ${CP_PATH_QCAN}/qcan_plugin.cpp
   ${CP_PATH_QCAN}/qcan_server.cpp
   ${CP_PATH_QCAN}/qcan_server_logger.cpp
   ${CP_PATH_QCAN}/qcan_server_logger_view.cpp
   ${CP_PATH_QCAN}/qcan_timestamp.cpp
)

list (APPEND RESOURCES server.qrc)




if (APPLE)
   find_library(OSX_FRAMEWORK Foundation)
   if (NOT OSX_FRAMEWORK)
      message(FATAL_ERROR "Foundation framework not found")
   endif()
endif()

#-------------------------------------------------------------------------------------------------------
# create binary from selected source files
#
if (APPLE)

   set(MACOSX_BUNDLE_ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/images/canpie_server.icns)

   # And this part tells CMake where to find and install the file itself
   set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/images/canpie_server.icns)
   set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

   add_executable(${PROJECT_NAME} MACOSX_BUNDLE
      ${APP_ICON}
      ${APP_SOURCES}
      ${QCAN_SOURCES}
      ${RESOURCES}
   )

else()
   add_executable(${PROJECT_NAME} WIN32
   ${APP_SOURCES}
   ${QCAN_SOURCES}
   ${RESOURCES}
   )

endif()


#-------------------------------------------------------------------------------------------------------
# link Qt libs 
#
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::WebSockets Qt${QT_VERSION_MAJOR}::Widgets)
if(APPLE)
   target_link_libraries(${PROJECT_NAME} "-framework Foundation")

   set_target_properties(${PROJECT_NAME} PROPERTIES
      OUTPUT_NAME "CANpieServer"
      MACOSX_BUNDLE TRUE
      MACOSX_RPATH TRUE
      MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
   )
   set(BINARY_NAME ${PROJECT_NAME}.app)

elseif(WIN32)
   set(BINARY_NAME ${PROJECT_NAME}.exe)
else()
   set(BINARY_NAME ${PROJECT_NAME})
endif()

#-------------------------------------------------------------------------------------------------------
# Installation for Unix systems 
#
# if (UNIX)
#    install(PROGRAMS ${CP_PATH_BIN}/${BINARY_NAME} BUNDLE DESTINATION /usr/local/bin)
# endif ()


