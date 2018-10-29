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

#---------------------------------------------------------------
# project configuration and compiler options
#
CONFIG += debug_and_release
CONFIG += warn_on
CONFIG += C++11
CONFIG += silent


#---------------------------------------------------------------
# list of sub directories
#
SUBDIRS  = ./can-dump    \
           ./can-error   \
           ./can-send    \
           ./server		 \
           ./plugins

message("--------------------------------------------------------------------------------")
message("Compile all QCan applications ..                                                ")
message("--------------------------------------------------------------------------------")
