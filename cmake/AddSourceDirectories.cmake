#----------------------------------------------------------------------------------------------------------------------#
# This CMake include defines the source directories and the source files for the project                               #
#                                                                                                                      #
# The following defintions are used:                                                                                   #
# CP_PATH_<name> : directory to source code files                                                                      #
# CP_FILE_<name> : source files (C/C++)                                                                                #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#


#-------------------------------------------------------------------------------------------------------
# set general paths of the project
#
set(CP_PATH_CMAKE             ${CMAKE_CURRENT_LIST_DIR})

#-------------------------------------------------------------------------------------------------------
# path for executables
#
set(CP_PATH_BIN               ${CP_PATH_CMAKE}/../bin)

#-------------------------------------------------------------------------------------------------------
# path for plugins
# Windows: bin/plugins
# Linux  : bin/plugins
# macOS  : bin/CANpieServer.app/Contents/PlugIns
#
if(APPLE)
   set(CP_PATH_PLUGINS        ${CP_PATH_BIN}/CANpieServer.app/Contents/PlugIns)
else()
   set(CP_PATH_PLUGINS        ${CP_PATH_BIN}/PlugIns)
endif()


#-------------------------------------------------------------------------------------------------------
# general source path
#
set(CP_PATH_SOURCE            ${CP_PATH_CMAKE}/../source)

set(CP_PATH_APPS              ${CP_PATH_SOURCE}/application)
set(CP_PATH_CANPIE            ${CP_PATH_SOURCE}/canpie-fd)
set(CP_PATH_DEVICE_QCAN       ${CP_PATH_SOURCE}/device/qcan)
set(CP_PATH_DEVICE_TEMPLATE   ${CP_PATH_APPS}/template-canpie-fd)

set(CP_PATH_MISC              ${CP_PATH_SOURCE}/misc)
set(CP_PATH_QCAN              ${CP_PATH_SOURCE}/qcan)

set(CP_PATH_TEST              ${CP_PATH_CMAKE}/../test)




list(
    APPEND CP_FILE_CANPIE
    ${CP_PATH_CANPIE}/cp_fifo.c
    ${CP_PATH_CANPIE}/cp_msg.c
)

list(
    APPEND CP_FILE_DEVICE_QCAN
    ${CP_PATH_DEVICE_QCAN}/qcan_socket_canpie_fd.cpp
)

list(
    APPEND CP_FILE_DEVICE_TEMPLATE
    ${CP_PATH_DEVICE_TEMPLATE}/device_canfd.c
)