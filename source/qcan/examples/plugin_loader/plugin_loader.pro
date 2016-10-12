#=============================================================================#
# File:          plugin_loader.pro                                            #
# Description:   example project to load plugins                              #
#                                                                             #
# Copyright (C) MicroControl GmbH & Co. KG                                    #
# 53844 Troisdorf - Germany                                                   #
# www.microcontrol.net                                                        #
#                                                                             #
#=============================================================================#

#---------------------------------------------------------------
# Name of QMake project
#
QMAKE_PROJECT_NAME = "Plugin Loader"

#---------------------------------------------------------------
# template type
#
TEMPLATE = app

#---------------------------------------------------------------
# Qt modules used
#
QT += widgets

#---------------------------------------------------------------
# target file name
#
TARGET       = PluginTalk


#---------------------------------------------------------------
# directory for target file
#
DESTDIR = ../../../../bin

#--------------------------------------------------------------------
# Objects directory
#

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
FORMS   =


#---------------------------------------------------------------
# resource collection files
#
RESOURCES =


#---------------------------------------------------------------
# include directory search path
#
INCLUDEPATH  = .
INCLUDEPATH += ./../..
INCLUDEPATH += ./../../../qcan
INCLUDEPATH += ./../../../canpie/version3

#---------------------------------------------------------------
# search path for source files
#
VPATH  = .
VPATH += ./../..
VPATH += ./../../../qcan
VPATH += ./../../../canpie/version3

#---------------------------------------------------------------
# header files of project
#
HEADERS  =  qcan_interface.hpp   \
            qcan_plugin.hpp      \
            qcan_frame.hpp       \
            plugin_loader.hpp

#---------------------------------------------------------------
# source files of project
#
SOURCES  =  plugin_loader_main.cpp       \
            plugin_loader.cpp            \
            canpie_frame_api.cpp       \
            canpie_frame_error.cpp     \
            canpie_frame.cpp           \
            canpie_timestamp.cpp       \
            qcan_frame.cpp

#---------------------------------------------------------------
# OS specific settings
#
macx {
   message("Building 'Plugin Loader' for Mac OS X ...")
}

win32 {
   message("Building 'Plugin Loader' for Windows ...")
}

