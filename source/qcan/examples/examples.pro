#=============================================================================#
# File:          examples.pro                                                 #
# Description:   qmake project file for CANpie examples                       #
#                                                                             #
# Copyright (C) MicroControl GmbH & Co. KG                                    #
# 53844 Troisdorf - Germany                                                   #
# www.microcontrol.net                                                        #
#                                                                             #
#=============================================================================#

#---------------------------------------------------------------
# Name of QMake project
#
QMAKE_PROJECT_NAME = "examples"

#---------------------------------------------------------------
# template type: run qmake projects in sub-directories
#
TEMPLATE = subdirs

#---------------------------------------------------------------
# list of sub directories
#
SUBDIRS  = ./plugin_loader

#           ./client
