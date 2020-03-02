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
QT += core network websockets testlib

#---------------------------------------------------------------
# target file name
#
TARGET = qcantest

#---------------------------------------------------------------
# directory for target file
#
DESTDIR = ./../../bin

#---------------------------------------------------------------
# Directory for intermediate moc files
# 
MOC_DIR = ./../../objs
RCC_DIR = ./../../objs

#--------------------------------------------------------------------
# Directory for object files
#
OBJECTS_DIR = ./../../objs

#---------------------------------------------------------------
# project configuration and compiler options
#
CONFIG += debug_and_release
CONFIG += warn_on
CONFIG += C++11
CONFIG += silent
CONFIG += testcase
CONFIG += console

#---------------------------------------------------------------
# suppress warnings caused by the Qt file qtestevent.h
#
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-private-field


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
INCLUDEPATH += ./../../source/canpie-fd
INCLUDEPATH += ./../../source/device/qcan
INCLUDEPATH += ./../../source/misc
INCLUDEPATH += ./../../source/qcan


#---------------------------------------------------------------
# search path for source files
#
VPATH  = .
VPATH += ./../../source/canpie-fd
VPATH += ./../../source/device/qcan
VPATH += ./../../source/qcan


#---------------------------------------------------------------
# header files of project 
#
HEADERS +=  qcan_frame.hpp                \
            qcan_interface.hpp            \
            qcan_socket.hpp               \
            qcan_socket_canpie_fd.hpp     \
            test_qcan_filter.hpp          \
            test_qcan_frame.hpp           \
            test_qcan_socket.hpp          \
            test_qcan_socket_canpie.hpp   \
            test_qcan_timestamp.hpp

#---------------------------------------------------------------
# source files of project 
#
SOURCES +=  qcan_filter.cpp               \
            qcan_filter_list.cpp          \
            qcan_frame.cpp                \
            qcan_socket.cpp               \
            qcan_socket_canpie_fd.cpp     \
            qcan_timestamp.cpp            \
            cp_fifo.c                     \
            cp_msg.c                      \
            test_qcan_filter.cpp          \
            test_qcan_frame.cpp           \
            test_qcan_socket.cpp          \
            test_qcan_socket_canpie.cpp   \
            test_qcan_timestamp.cpp       \
            test_main.cpp




            
#---------------------------------------------------------------
# OS specific settings
#
macx {
   message("Building '$$QMAKE_PROJECT_NAME' for Mac OS X ...")
   QMAKE_MAC_SDK = macosx10.12
   QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
   
   #--------------------------------------------------
   # do not create application bundle
   #
   CONFIG -= app_bundle
}            