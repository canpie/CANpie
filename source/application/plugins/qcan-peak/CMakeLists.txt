#----------------------------------------------------------------------------------------------------------------------#
# CMake file plugin QCanPeak                                                                                           #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

message("-- Configure plugin QCanPeak")

#-------------------------------------------------------------------------------------------------------
# define the project name and version
#
project(CANpiePluginPeak VERSION 1.00.2)


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
   find_package(Qt5 QUIET COMPONENTS Core Widgets )
   if (Qt5_FOUND)
      set(QT_VERSION_MAJOR 5)
      message("-- Found Qt version 5" )
   endif()
endif()


if (${QT_VERSION_MAJOR} EQUAL 0)
   find_package(Qt6 QUIET COMPONENTS Core Widgets )
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
# Check if PEAK header files are present
#
if(APPLE)
   if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/macos/PCBUSB.h")
      message("   :: Header file for PEAK API found")
      set(GENERATE_QCAN_PEAK  1)
      include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/macos)
   else()
      message("   :: Header file for PEAK API is missing - plugin not generated")
      set(GENERATE_QCAN_PEAK  0)
   endif()
endif()

if(WIN32)
   if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/windows/PCANBasic.h")
      message("   :: Header file for PEAK API found")
      set(GENERATE_QCAN_PEAK  1)
      include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/windows)
   else()
      message("   :: Header file for PEAK API is missing - plugin not generated")
      set(GENERATE_QCAN_PEAK  0)
   endif()
endif()

if(${GENERATE_QCAN_PEAK} EQUAL 0)
   message(STATUS "-------------------------------------------------------------------------")
   message(STATUS "Please check the README.md file in the directory"                         )
   message(STATUS " ${CMAKE_CURRENT_SOURCE_DIR} "                                            )
   message(STATUS "for instructions how to get the required header file."                    )
   message(STATUS "-------------------------------------------------------------------------")
endif()

#------------------------------------------------------------------------------------------------------- 
# define source files for compilation
#
list(
   APPEND PLUGIN_SOURCES
   qcan_interface_peak.cpp
   qcan_pcan_basic.cpp
   qcan_plugin_peak.cpp
)

list(
   APPEND QCAN_SOURCES
   ${CP_PATH_QCAN}/qcan_frame.cpp
   ${CP_PATH_QCAN}/qcan_plugin.cpp
   ${CP_PATH_QCAN}/qcan_timestamp.cpp
)

list (APPEND RESOURCES qcan_peak.qrc)



#-------------------------------------------------------------------------------------------------------
# create plugin from selected source files
#
if(${GENERATE_QCAN_PEAK} EQUAL 1)

   add_library(${PROJECT_NAME} SHARED ${PLUGIN_SOURCES} ${QCAN_SOURCES} ${RESOURCES})


   #-----------------------------------------------------------------------------------------------
   # link Qt libs 
   #
   if(APPLE)
      target_link_directories(${PROJECT_NAME} PUBLIC ./lib/macos)
      target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets PCBUSB)
   else()
      target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets)
   endif()

   #------------------------------------------------------------------------------------------------
   # copy library to plugins directory:
   #
   set(LIB_NAME ${CMAKE_SHARED_LIBRARY_PREFIX}${PROJECT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX})

   add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                      COMMAND ${CMAKE_COMMAND} -E echo "       Copy ${LIB_NAME} to ${CP_PATH_PLUGINS}/"
                      COMMAND ${CMAKE_COMMAND} -E make_directory ${CP_PATH_PLUGINS} 
                      COMMAND ${CMAKE_COMMAND} -E copy ${LIB_NAME} ${CP_PATH_PLUGINS}/)

   #------------------------------------------------------------------------------------------------
   # copy the library for OS X to the Contents/Frameworks directory
   # make sure to adjust the version number each time the library gets an update
   #
   if (APPLE)
      set(MAC_CAN_LIB_VERSION 0.11.2)
      set(MAC_CAN_LIB_FILE libPCBUSB.${MAC_CAN_LIB_VERSION}.dylib)
      add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E echo "       Copy ${MAC_CAN_LIB_FILE} to ${CP_PATH_FRAMEWORKS}/"
                        COMMAND ${CMAKE_COMMAND} -E make_directory ${CP_PATH_FRAMEWORKS} 
                        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lib/macos/${MAC_CAN_LIB_FILE} 
                                                         ${CP_PATH_FRAMEWORKS}/libPCBUSB.dylib)

   endif()

endif()
