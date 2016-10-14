#=============================================================================#
# File:          client.pro                                                   #
# Description:   qmake project file for QCAN client example                   #
#                                                                             #
# Copyright (C) MicroControl GmbH & Co. KG                                    #
# 53844 Troisdorf - Germany                                                   #
# www.microcontrol.net                                                        #
#                                                                             #
#=============================================================================#

#---------------------------------------------------------------
# Name of QMake project
#
QMAKE_PROJECT_NAME = "QCanClient"

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
TARGET = QCanClient

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
VERSION = 0.63.2


#---------------------------------------------------------------
# definitions for preprocessor
#
DEFINES =  "MC_TARGET=MC_OS_LINUX"


#---------------------------------------------------------------
# UI files
#
FORMS   =  ./forms/ClientDemo.ui


#---------------------------------------------------------------
# resource collection files 
#
RESOURCES = 



#---------------------------------------------------------------
# include directory search path
#
INCLUDEPATH  = .
INCLUDEPATH += ./../../
INCLUDEPATH += ./../../../canpie/version3/

#---------------------------------------------------------------
# search path for source files
#
VPATH  = .
VPATH += ./../../
VPATH += ./../../../canpie/version3


#---------------------------------------------------------------
# header files of project 
#
HEADERS += client_demo.hpp       \
           qcan_frame.hpp        \
           qcan_interface.hpp    \
           qcan_socket.hpp

            
#---------------------------------------------------------------
# source files of project 
#
SOURCES +=  client_demo.cpp         \
            client_main.cpp         \
            canpie_frame_api.cpp    \
            canpie_frame_error.cpp  \
            canpie_frame.cpp        \
            canpie_timestamp.cpp    \
            qcan_frame.cpp          \
            qcan_frame_api.cpp      \
            qcan_frame_error.cpp    \
            qcan_socket.cpp
            
H


#---------------------------------------------------------------
# OS specific settings 
#
macx {
   message("Building '$$QMAKE_PROJECT_NAME' for Mac OS X ...")

   
   #--------------------------------------------------
   # The correct version of the MAC SDK might be 
   # necessary depending on the combination of
   # Qt version and Mac OS X (i.e. Xcode) version.
   # For macOS Sierra (Xcode 8) in combination with
   # Qt 5.6.0 the following definition is required.
   # The active SDK version can be looked up by checking 
   # the symbolic link in this directory:
   # /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/
   #
   QMAKE_MAC_SDK = macosx10.12
   
   #--------------------------------------------------
   # Minimum OS X version for submission is 10.9
   #
   QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
   
   #--------------------------------------------------
   # Icon for application
   #
   #ICON = ./images/server.icns
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
