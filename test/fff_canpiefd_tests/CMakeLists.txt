#------------------------------------------------------------------------------
# specify minimum version of CMake that can be used with CMakeLists.txt file
#
cmake_minimum_required(VERSION 3.16.3)

#------------------------------------------------------------------------------
# This project consists of a CMakeLists.txt and can be translated with the 
# following commands:
# > mkdir build
# > cd build
# > cmake ..
# > cmake --build .
# 
# It supports CMake testing:
# > ctest
#
message("-- Configure FFF based test cases for CANpie-FD")

#------------------------------------------------------------------------------
# specify project
#
project(test-canpiefd)

#------------------------------------------------------------------------------
# set general paths of the project
#
set(PROJECT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(TEST_PATH  ${PROJECT_PATH}/test)

#------------------------------------------------------------------------------
# specify required CANpie source files
#
list(APPEND cp_sources
   ${PROJECT_PATH}/source/canpie-fd/cp_fifo.c
   ${PROJECT_PATH}/source/canpie-fd/cp_msg.c
   ${PROJECT_PATH}/source/canpie-fd/cp_nrl.c
)

list(APPEND fff_cp_fake
   ${TEST_PATH}/fff_cpcore/ff_cp_core_nrl.c
)
#------------------------------------------------------------------------------
# specify project source files
#
list(APPEND test_cp_sources
   test_cp_main.c
   test_cp_fifo.c
   test_cp_nrl_mbx.c
   test_cp_nrl.c
)

#------------------------------------------------------------------------------
# specify include paths 
#
include_directories(${PROJECT_PATH}/source/hal)
include_directories(${PROJECT_PATH}/source/misc)
include_directories(${PROJECT_PATH}/source/canpie-fd)

include_directories(${TEST_PATH}/fff)
include_directories(${TEST_PATH}/fff_mc)
include_directories(${TEST_PATH}/fff_canpiefd_tests)
include_directories(${TEST_PATH}/fff_cpcore)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#------------------------------------------------------------------------------
# specify some CMAKE variables 
#
add_executable(${PROJECT_NAME}
   ${cp_sources}
   ${fff_cp_fake}
   ${test_cp_sources}
)

#------------------------------------------------------------------------------
# add some test
#
enable_testing()
add_test(${PROJECT_NAME} ${PROJECT_NAME})