#=============================================================================#
# File:          qcan_ixxat.pro                                               #
# Description:   qmake project file for IXXAT plugin                          #
#                                                                             #
# Copyright (C) MicroControl GmbH & Co. KG                                    #
# 53844 Troisdorf - Germany                                                   #
# www.microcontrol.net                                                        #
#                                                                             #
#=============================================================================#


#---------------------------------------------------------------
# Name of QMake project
#
QMAKE_PROJECT_NAME = "QCan IXXAT"

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
TARGET   = $$qtLibraryTarget(QCanIxxat)

#---------------------------------------------------------------
# directory for target file
#
DESTDIR  = ../../../../../bin/plugins

#---------------------------------------------------------------
# Objects directory
#
OBJECTS_DIR = ./objs/

#---------------------------------------------------------------
# project configuration and compiler options
#
CONFIG += release
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
RESOURCES = qcan_ixxat.qrc

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
            qcan_interface_ixxat.hpp\
            qcan_ixxat_vci.hpp      \
            qcan_plugin.hpp         \
            qcan_plugin_ixxat.hpp


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
            qcan_interface_ixxat.cpp\
            qcan_ixxat_vci.cpp      \
            qcan_plugin_ixxat.cpp

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
   message("Building '$$QMAKE_PROJECT_NAME' for Windows ...")
   CONFIG(debug, debug|release) {

   } else {
      DEFINES += QT_NO_WARNING_OUTPUT
      DEFINES += QT_NO_DEBUG_OUTPUT
   }
}

