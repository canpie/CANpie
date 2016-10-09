#=============================================================================#
# File:          server.pro                                                   #
# Description:   qmake project file for QCAN server                           #
#                                                                             #
# Copyright (C) MicroControl GmbH & Co. KG                                    #
# 53844 Troisdorf - Germany                                                   #
# www.microcontrol.net                                                        #
#                                                                             #
#=============================================================================#


#---------------------------------------------------------------
# template type
#
TEMPLATE = app

#---------------------------------------------------------------
# Qt modules used
#
QT += core gui network widgets

#---------------------------------------------------------------
# target file name
#
TARGET = CANpieServer

#---------------------------------------------------------------
# directory for target file
#
DESTDIR = ../../../../bin

#--------------------------------------------------------------------
# Objects directory
#
OBJECTS_DIR = ./objs/

#---------------------------------------------------------------
# project configuration and compiler options
#
CONFIG += debug
CONFIG += warn_on
CONFIG += C++11
CONFIG += silent

#---------------------------------------------------------------
# version of the application
#
VERSION = 0.82.1

#---------------------------------------------------------------
# definitions for preprocessor
#
DEFINES =  

#---------------------------------------------------------------
# UI files
#
FORMS   =  ./forms/ServerConfig.ui


#---------------------------------------------------------------
# resource collection files 
#
RESOURCES = server.qrc


#---------------------------------------------------------------
# include directory search path
#
INCLUDEPATH  = .
INCLUDEPATH += ../../
INCLUDEPATH += ../../../canpie/version3/

#---------------------------------------------------------------
# search path for source files
#
VPATH  = .
VPATH += ./../..
VPATH += ./../../../canpie/version3

#---------------------------------------------------------------
# header files of project 
#
HEADERS =   qcan_server_dialog.hpp     \
            qcan_interface_widget.hpp  \
            qcan_frame.hpp             \
            qcan_interface.hpp         \
            qcan_network.hpp           \
            qcan_server.hpp
                
            
#---------------------------------------------------------------
# source files of project 
#
SOURCES =   qcan_server_dialog.cpp     \
            qcan_interface_widget.cpp  \
            server_main.cpp            \
            canpie_frame_api.cpp       \
            canpie_frame_error.cpp     \
            canpie_frame.cpp           \
            canpie_timestamp.cpp       \
            qcan_frame.cpp             \
            qcan_frame_api.cpp         \
            qcan_frame_error.cpp       \
            qcan_network.cpp           \
            qcan_server.cpp


#---------------------------------------------------------------
# OS specific settings 
#
macx {
   message("Building for Mac OS X ...")
   QMAKE_MAC_SDK = macosx10.12
   QMAKE_INFO_PLIST = Info.plist
   # Minimum OS X version for submission is 10.9
   QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
   
   # Icon for application
   ICON = ./images/server.icns
}

win32 {
   message("Building for WIN 32 ...")
   RC_FILE += server.rc
}
