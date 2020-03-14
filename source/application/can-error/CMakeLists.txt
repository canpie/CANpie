#----------------------------------------------------------------------------------------------------------------------#
# CMake file for can-error                                                                                             #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.16.4 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

message("-- Configure can-error")

#-------------------------------------------------------------------------------------------------------
# define the project name and version
#
project(can-error VERSION 0.99.8)


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



set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core Network WebSockets REQUIRED)


#-------------------------------------------------------------------------------------------------------
# create binary from selected source files
#
add_executable(${PROJECT_NAME} 
    ${APP_SOURCES}
    ${QCAN_SOURCES}

)

#-------------------------------------------------------------------------------------------------------
# link Qt libs 
#
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network Qt5::WebSockets)

#-------------------------------------------------------------------------------------------------------
# copy program to bin directory 
#
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E echo "       Copy ${PROJECT_NAME} to ${CP_PATH_BIN}"
                   COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_NAME} ${CP_PATH_BIN})
