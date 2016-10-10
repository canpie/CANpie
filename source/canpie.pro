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
# template type: run qmake projects in sub-directories
#
TEMPLATE = subdirs

#---------------------------------------------------------------
# list of sub directories
#
SUBDIRS  = ./qcan/applications/server                 \
           ./qcan/applications/plugins/qcan_peakusb   \
           ./tests


#           ./qcan/applications/plugins/qcan_ixxatusb  \
