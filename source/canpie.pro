#=============================================================================#
# File:          canpie.pro                                                   #
# Description:   qmake project file for CANpie                                #
#                                                                             #
# Copyright (C) MicroControl GmbH & Co. KG                                    #
# 53844 Troisdorf - Germany                                                   #
# www.microcontrol.net                                                        #
#                                                                             #
#=============================================================================#

#---------------------------------------------------------------
# Name of QMake project
#
QMAKE_PROJECT_NAME = "CANpie"

#---------------------------------------------------------------
# template type: run qmake projects in sub-directories
#
TEMPLATE = subdirs

#---------------------------------------------------------------
# list of sub directories
#
SUBDIRS  = ./qcan/applications \
           ./qcan/examples
