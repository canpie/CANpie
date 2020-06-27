#=============================================================================#
# File:          qcan-template.pro                                            #
# Description:   qmake project file for template plugin                       #
#                                                                             #
# Copyright (C) MicroControl GmbH & Co. KG                                    #
# 53844 Troisdorf - Germany                                                   #
# www.microcontrol.net                                                        #
#                                                                             #
#=============================================================================#


#---------------------------------------------------------------
# Name of QMake project
#
QMAKE_PROJECT_NAME = "QCan Template"

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
TARGET          = $$qtLibraryTarget(QCanTemplate)

#---------------------------------------------------------------
# directory for target file
#
macx {
   DESTDIR = ../../../../bin/CANpieServer.app/Contents/PlugIns
}
win32 {
   DESTDIR = ../../../../bin/plugins
}

#---------------------------------------------------------------
# Objects directory
#
OBJECTS_DIR = ./objs
MOC_DIR     = ./objs
RCC_DIR     = ./objs

#---------------------------------------------------------------
# project configuration and compiler options
#
CONFIG += debug_and_release
CONFIG += plugin
CONFIG += warn_on
CONFIG += C++11
CONFIG += silent

#---------------------------------------------------------------
# version of the application
#
VERSION_MAJOR = 1
VERSION_MINOR = 00
VERSION_BUILD = 02


#---------------------------------------------------------------
# Target version
#
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}


#---------------------------------------------------------------
# definitions for preprocessor
#
DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
           "VERSION_MINOR=$$VERSION_MINOR"\
           "VERSION_BUILD=$$VERSION_BUILD"\
           "TARGET_NAME=$$TARGET"


#---------------------------------------------------------------
# UI files
#
FORMS   =

#---------------------------------------------------------------
# resource collection files
#
RESOURCES = qcan_template.qrc

#---------------------------------------------------------------
# include directory search path
#
INCLUDEPATH  = .
INCLUDEPATH += ./../../../qcan


#---------------------------------------------------------------
# search path for source files
#
VPATH  = .
VPATH += ./../../../qcan


#---------------------------------------------------------------
# header files of project
#
HEADERS =   qcan_interface.hpp          \
            qcan_interface_template.hpp \
            qcan_plugin.hpp             \
            qcan_plugin_template.hpp


#---------------------------------------------------------------
# source files of project
#
SOURCES =   qcan_frame.cpp              \
            qcan_timestamp.cpp          \
            qcan_interface_template.cpp \
            qcan_plugin_template.cpp


EXAMPLE_FILES = plugin.json

#---------------------------------------------------------------
# OS specific settings
#
macx {
   QMAKE_MAC_SDK = macosx10.12
   QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9

   CONFIG(debug, debug|release) {
      message("Building '$$QMAKE_PROJECT_NAME' DEBUG version for Mac OS X ...")
   } else {
      message("Building '$$QMAKE_PROJECT_NAME' RELEASE version for Mac OS X ...")
      DEFINES += QT_NO_WARNING_OUTPUT
      DEFINES += QT_NO_DEBUG_OUTPUT
      DEFINES += QT_NO_INFO_OUTPUT
   }
}

win32 {
   CONFIG(debug, debug|release) {
      message("Building '$$QMAKE_PROJECT_NAME' DEBUG version for Windows ...")
   } else {
      message("Building '$$QMAKE_PROJECT_NAME' RELEASE version for Windows ...")
      DEFINES += QT_NO_WARNING_OUTPUT
      DEFINES += QT_NO_DEBUG_OUTPUT
      DEFINES += QT_NO_INFO_OUTPUT
   }
}


