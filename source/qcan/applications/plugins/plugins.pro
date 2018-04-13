#=============================================================================#
# File:          plugins.pro                                                  #
# Description:   qmake project file for CANpie application plugins            #
#                                                                             #
# Copyright (C) MicroControl GmbH & Co. KG                                    #
# 53844 Troisdorf - Germany                                                   #
# www.microcontrol.net                                                        #
#                                                                             #
#=============================================================================#

#---------------------------------------------------------------
# Name of QMake project
#
QMAKE_PROJECT_NAME = "CANpie plugins"

#---------------------------------------------------------------
# template type: run qmake projects in sub-directories
#
TEMPLATE = subdirs

CONFIG += debug_and_release 

#---------------------------------------------------------------
# list of sub directories
#
SUBDIRS  =  ./qcan_peak 


#---------------------------------------------------------------
# IXXAT plugin is only available for Windows
#
win32 {
	SUBDIRS  +=  ./qcan_ixxat
}


