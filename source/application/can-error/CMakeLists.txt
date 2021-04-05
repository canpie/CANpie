#----------------------------------------------------------------------------------------------------------------------#
# CMake file for can-error                                                                                             #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

message("-- Configure can-error")

#-------------------------------------------------------------------------------------------------------
# define the project name and version
#
project(can-error VERSION 0.99.10)


#-------------------------------------------------------------------------------------------------------
# add project CMake module include path
#
set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake" ${CMAKE_MODULE_PATH})
include(AddCompilerFlags )
include(AddSourceDirectories )


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
# Use C++ 17 standard
#
set(CMAKE_CXX_STANDARD 17)


#-------------------------------------------------------------------------------------------------------
# Configure Qt support
#
set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Core Network WebSockets REQUIRED)


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
if(NOT CMAKE_BUILD_TYPE MATCHES Debug)
   add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()


#------------------------------------------------------------------------------------------------------- 
# define source files for compilation
#
list(
   APPEND APP_SOURCES
   qcan_error.cpp
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
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network Qt5::WebSockets)

#-------------------------------------------------------------------------------------------------------
# Copy program to bin directory 
#
if (UNIX)
   install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} DESTINATION /usr/local/bin)
endif ()
