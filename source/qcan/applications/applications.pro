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
QMAKE_PROJECT_NAME = "applications"

#---------------------------------------------------------------
# template type: run qmake projects in sub-directories
#
TEMPLATE = subdirs

CONFIG += debug_and_release 

#---------------------------------------------------------------
# list of sub directories
#
SUBDIRS  = ./can-config  \
           ./can-dump    \
           ./can-error   \
           ./can-send    \
           ./server		 \
           ./plugins

message("--------------------------------------------------------------------------------")
message("Compile all QCan applications ..                                                ")
message("--------------------------------------------------------------------------------")
