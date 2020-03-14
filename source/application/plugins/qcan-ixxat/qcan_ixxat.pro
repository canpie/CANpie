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
# compile plugin only if necessary header file exists
#
macx {
   DRIVER_API =
}
win32 {
   DRIVER_API = ./include/vcinpl.h
}
exists($$DRIVER_API) {

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
macx {
   DESTDIR = ../../../../bin/CANpieServer.app/Contents/Plugins
}
win32 {
   DESTDIR = ../../../../bin/plugins
}

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
INCLUDEPATH += ./../../../qcan


#---------------------------------------------------------------
# search path for source files
#
VPATH  = .
VPATH += ./../../../qcan


#---------------------------------------------------------------
# header files of project
#
HEADERS =   qcan_interface.hpp         \
            qcan_interface_ixxat.hpp   \
            qcan_ixxat_vci.hpp         \
            qcan_plugin.hpp            \
            qcan_plugin_ixxat.hpp


#---------------------------------------------------------------
# source files of project
#
SOURCES =   qcan_data.cpp              \
            qcan_frame.cpp             \
            qcan_frame_api.cpp         \
            qcan_frame_error.cpp       \
            qcan_timestamp.cpp         \
            qcan_interface_ixxat.cpp   \
            qcan_ixxat_vci.cpp         \
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
   CONFIG(debug, debug|release) {
      message("Building '$$QMAKE_PROJECT_NAME' DEBUG version for Windows ...")
   } else {
      message("Building '$$QMAKE_PROJECT_NAME' RELEASE version for Windows ...")
      DEFINES += QT_NO_WARNING_OUTPUT
      DEFINES += QT_NO_INFO_OUTPUT
      DEFINES += QT_NO_DEBUG_OUTPUT
   }
}

#---------------------------------------------------------------
# necessary header file does not exist!
#
} else {
   message(" ");
   message(" '$$QMAKE_PROJECT_NAME' ommited from build, because IXXAT VCI 3.5 API is not available!");
   message(" Make sure './include' contains IXXAT VCI 3.5 API.");
   message(" Copy or link content of 'IXXAT/VCI 3.5/sdk/Microsoft_VisualC/inc' to the './include' folder.");
   message(" IXXAT VCI 3.5 API is included by 'VCI V3 driver' and can be downloaded from https://www.ixxat.com");
   message(" ");
}
