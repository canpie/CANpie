#----------------------------------------------------------------------------------------------------------------------#
# This CMake include defines compiler flags for the project                                                            #
#                                                                                                                      #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
#message("C++ compiler flags: ${CMAKE_CXX_FLAGS}")