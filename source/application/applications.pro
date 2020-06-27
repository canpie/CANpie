#=============================================================================#
# File:          applications.pro                                             #
# Description:   qmake project file for CANpie application                    #
#                                                                             #
# Copyright (C) MicroControl GmbH & Co. KG                                    #
# 53844 Troisdorf - Germany                                                   #
# www.microcontrol.net                                                        #
#                                                                             #
#=============================================================================#

#---------------------------------------------------------------
# Name of QMake project
#
QMAKE_PROJECT_NAME = "QCan applications"

#---------------------------------------------------------------
# template type: run qmake projects in sub-directories
#
TEMPLATE = subdirs

CONFIG  += debug_and_release

#---------------------------------------------------------------
# list of sub directories depending on used OS
#
SUBDIRS  = ./can-config    \
           ./can-dump      \
           ./can-error     \
           ./can-send 

macx {
SUBDIRS += ./server        \
           ./plugins
}

win32 {
SUBDIRS += ./server        \
           ./plugins
}


message("--------------------------------------------------------------------------------")
message("Compile all QCan applications ..                                                ")
message("--------------------------------------------------------------------------------")
