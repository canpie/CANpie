#=============================================================================#
# File:          qcan_usart.pro                                               #
# Description:   qmake project file for CANpie to USART plugin                #
#                                                                             #
# Copyright (C) MicroControl GmbH & Co. KG                                    #
# 53844 Troisdorf - Germany                                                   #
# www.microcontrol.net                                                        #
#                                                                             #
#=============================================================================#


#---------------------------------------------------------------
# Name of QMake project
#
QMAKE_PROJECT_NAME = "QCan USART"

#---------------------------------------------------------------
# template type
# app   if you want run plugin standalone in a QWidget
# lib   final library, which have to be loaded by an app
#
TEMPLATE = lib

#---------------------------------------------------------------
# Qt modules used
#
contains(TEMPLATE, app) {
QT      += core gui
}
QT      += widgets serialport

#---------------------------------------------------------------
# target file name
#
contains(TEMPLATE, app) {
TARGET          = QCanUsart
} else {
TARGET          = $$qtLibraryTarget(QCanUsart)
}

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
VERSION_MINOR = 02
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
# definition for number of channels
#
DEFINES  += MC_USART_PORT_MAX=CP_CHANNEL_MAX

#---------------------------------------------------------------
# UI files
#
FORMS     = qcan_config.ui


#---------------------------------------------------------------
# resource collection files
#
RESOURCES = qcan_usart.qrc

#---------------------------------------------------------------
# include directory search path
#
INCLUDEPATH  = .
INCLUDEPATH += ./forms
INCLUDEPATH += ./../../../canpie-fd
INCLUDEPATH += ./../../../misc
INCLUDEPATH += ./../../../qcan


#---------------------------------------------------------------
# search path for source files
#
VPATH  = .
VPATH += ./forms
VPATH += ./../../../qcan
VPATH += ./../../../canpie-fd


#---------------------------------------------------------------
# header files of project
#
HEADERS =   qcan_interface.hpp         \
            qcan_interface_usart.hpp   \
            qcan_config.hpp            \
            qcan_usart.hpp             \
            qcan_plugin.hpp            \
            qcan_plugin_usart.hpp

HEADERS +=  cp_platform.h  \
            mc_usart.h

#---------------------------------------------------------------
# source files of project
#
SOURCES =   qcan_frame.cpp             \
            qcan_timestamp.cpp         \
            qcan_interface_usart.cpp   \
            qcan_config.cpp            \
            qcan_usart.cpp             \
            qcan_plugin_usart.cpp


SOURCES += cp_fifo.c    \
           cp_msg.c     \
           cp_usart.c


contains(TEMPLATE, app) {
SOURCES += qcan_main.cpp
}

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
