#----------------------------------------------------------------------------------------------------------------------#
# This CMake include defines the source directories and the source files for the project                               #
#                                                                                                                      #
# The following defintions are used:                                                                                   #
# CP_PATH_<name> : directory to source code files                                                                      #
# CP_FILE_<name> : source files (C/C++)                                                                                #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
#message("C++ compiler flags: ${CMAKE_CXX_FLAGS}")