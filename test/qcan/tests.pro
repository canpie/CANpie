#=============================================================================#
# File:          tests.pro                                                    #
# Description:   qmake project file for QCan class testing                    #
#                                                                             #
# Copyright (C) MicroControl GmbH & Co. KG                                    #
# 53844 Troisdorf - Germany                                                   #
# www.microcontrol.net                                                        #
#                                                                             #
#=============================================================================#

#---------------------------------------------------------------
# Name of QMake project
#
QMAKE_PROJECT_NAME = "QCan class testing"

#---------------------------------------------------------------
# template type
#
TEMPLATE = app

#---------------------------------------------------------------
# Qt modules used
#
QT += core network testlib

#---------------------------------------------------------------
# target file name
#
TARGET = qcantest

#---------------------------------------------------------------
# directory for target file
#
#DESTDIR = ../../../../bin

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
CONFIG += testcase

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
INCLUDEPATH += .
INCLUDEPATH += ./../../qcan


#---------------------------------------------------------------
# search path for source files
#
VPATH  = .
VPATH += ./../../qcan


#---------------------------------------------------------------
# header files of project 
#
HEADERS +=  qcan_frame.hpp             \
            qcan_interface.hpp         \
            qcan_socket.hpp            \
            test_qcan_frame.hpp        \
            test_qcan_socket.hpp       \
            test_qcan_timestamp.hpp

#---------------------------------------------------------------
# source files of project 
#
SOURCES +=  qcan_data.cpp              \
            qcan_frame.cpp             \
            qcan_frame_api.cpp         \
            qcan_frame_error.cpp       \
            qcan_timestamp.cpp         \
            qcan_socket.cpp            \
            test_qcan_frame.cpp        \
            test_qcan_socket.cpp       \
            test_qcan_timestamp.cpp    \
            test_main.cpp




            
#---------------------------------------------------------------
# OS specific settings
#
macx {
   message("Building '$$QMAKE_PROJECT_NAME' for Mac OS X ...")
   QMAKE_MAC_SDK = macosx10.12
   QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
}            