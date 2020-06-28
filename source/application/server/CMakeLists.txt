#----------------------------------------------------------------------------------------------------------------------#
# CMake file for CANpieServer                                                                                          #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.16.4 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

message("-- Configure CANpieServer")

#-------------------------------------------------------------------------------------------------------
# define the project name and version
#
project(CANpieServer VERSION 0.99.8)


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
   APPEND APP_SOURCES
   qcan_interface_widget.cpp
   qcan_server_dialog.cpp
   server_main.cpp
)


#------------------------------------------------------------------------------------------------------- 
# On macOSdefine source files for compilation
#
if (APPLE)
   list(APPEND APP_SOURCES  MacAppNapSuspender.mm)
endif()

list(
   APPEND QCAN_SOURCES
   ${CP_PATH_QCAN}/qcan_frame.cpp
   ${CP_PATH_QCAN}/qcan_network.cpp
   ${CP_PATH_QCAN}/qcan_server.cpp
   ${CP_PATH_QCAN}/qcan_server_logger.cpp
   ${CP_PATH_QCAN}/qcan_server_logger_view.cpp
   ${CP_PATH_QCAN}/qcan_timestamp.cpp
)

list (APPEND RESOURCES server.qrc)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ./forms)


find_package(Qt5 COMPONENTS Core Gui Network WebSockets Widgets REQUIRED)

if (APPLE)
   find_library(OSX_FRAMEWORK Foundation)
   if (NOT OSX_FRAMEWORK)
      message(FATAL_ERROR "Foundation framework not found")
   endif()

   set(ICON_FILE  ${CMAKE_CURRENT_SOURCE_DIR}/images/canpie_server.icns)
   set_source_files_properties(${ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

#-------------------------------------------------------------------------------------------------------
# create binary from selected source files
#
add_executable(${PROJECT_NAME} MACOSX_BUNDLE
   ${APP_SOURCES}
   ${QCAN_SOURCES}
   ${RESOURCES}
   ${ICON_FILE}
)


#-------------------------------------------------------------------------------------------------------
# link Qt libs 
#
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Network Qt5::WebSockets Qt5::Widgets)
if(APPLE)
   target_link_libraries(${PROJECT_NAME} "-framework Foundation")

   set_target_properties(${PROJECT_NAME} PROPERTIES
      OUTPUT_NAME "CANpieServer"
      MACOSX_BUNDLE TRUE
      MACOSX_RPATH TRUE
      MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
   )

    set(BINARY_NAME ${PROJECT_NAME}.app
)
else()
    set(BINARY_NAME ${PROJECT_NAME})
endif()

#-------------------------------------------------------------------------------------------------------
# copy program to bin directory 
#
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E echo "       Copy ${BINARY_NAME} to ${CP_PATH_BIN}"
                   COMMAND ${CMAKE_COMMAND} -E copy ${BINARY_NAME} ${CP_PATH_BIN})
