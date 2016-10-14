#=============================================================================#
# File:          qcan_peak.pro                                                #
# Description:   qmake project file for Peak plugin                           #
#                                                                             #
# Copyright (C) MicroControl GmbH & Co. KG                                    #
# 53844 Troisdorf - Germany                                                   #
# www.microcontrol.net                                                        #
#                                                                             #
#=============================================================================#


#---------------------------------------------------------------
# Name of QMake project
#
QMAKE_PROJECT_NAME = "QCan PEAK"

#---------------------------------------------------------------
# template type
#
TEMPLATE = lib

#---------------------------------------------------------------
# Qt modules used
#
QT      += widgets

#---------------------------------------------------------------
# target file name
#
TARGET          = $$qtLibraryTarget(QCanPeak)

#---------------------------------------------------------------
# directory for target file
#
DESTDIR = ../../../../../bin/plugins

#---------------------------------------------------------------
# Objects directory
#
OBJECTS_DIR = ./objs/

#---------------------------------------------------------------
# project configuration and compiler options
#
CONFIG += debug
CONFIG += plugin
CONFIG += warn_on
CONFIG += C++11
CONFIG += silent

#---------------------------------------------------------------
# version of the plugin
#
VERSION = 0.82.1

#---------------------------------------------------------------
# definitions for preprocessor
#
DEFINES =

#---------------------------------------------------------------
# UI files
#
FORMS   =

#---------------------------------------------------------------
# resource collection files
#
RESOURCES = qcan_peak.qrc

#---------------------------------------------------------------
# include directory search path
#
INCLUDEPATH  = .
INCLUDEPATH += ./include
INCLUDEPATH += ./../../..
INCLUDEPATH += ./../../../../canpie/version3

#---------------------------------------------------------------
# search path for source files
#
VPATH  = .
VPATH += ./../../..
VPATH += ./../../../../canpie/version3


#---------------------------------------------------------------
# header files of project
#
HEADERS =   qcan_frame_api.hpp      \
            qcan_frame_error.hpp    \
            qcan_frame.hpp          \
            qcan_interface.hpp      \
            qcan_interface_peak.hpp \
            qcan_pcan_basic.hpp     \
            qcan_plugin.hpp         \
            qcan_plugin_peak.hpp


#---------------------------------------------------------------
# source files of project
#
SOURCES =   canpie_frame_api.cpp    \
            canpie_frame_error.cpp  \
            canpie_frame.cpp        \
            canpie_timestamp.cpp    \
            qcan_frame_api.cpp      \
            qcan_frame_error.cpp    \
            qcan_frame.cpp          \
            qcan_interface_peak.cpp \
            qcan_pcan_basic.cpp     \
            qcan_plugin_peak.cpp


EXAMPLE_FILES = plugin.json

#---------------------------------------------------------------
# OS specific settings
#
macx {
   message("Building '$$QMAKE_PROJECT_NAME' for Mac OS X ...")
   QMAKE_MAC_SDK = macosx10.12
   QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
}

win32 {
   CONFIG(debug, debug|release) {
      message("Building '$$QMAKE_PROJECT_NAME' DEBUG version for Windows ...")
   } else {
      message("Building '$$QMAKE_PROJECT_NAME' RELEASE version for Windows ...")
      DEFINES += QT_NO_WARNING_OUTPUT
      DEFINES += QT_NO_DEBUG_OUTPUT
   }
}


