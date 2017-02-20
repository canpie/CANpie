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
# Name of QMake project
#
QMAKE_PROJECT_NAME = "CANpie Server"

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
CONFIG += debug release
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
INCLUDEPATH += ./../../
INCLUDEPATH += ./../../../qcan
INCLUDEPATH += ./../../../canpie-fd

#---------------------------------------------------------------
# search path for source files
#
VPATH  = .
VPATH += ./../..
VPATH += ./../../../qcan
VPATH += ./../../../canpie-fd

#---------------------------------------------------------------
# header files of project 
#
HEADERS =   qcan_interface_widget.hpp  \
            qcan_interface.hpp         \
            qcan_network.hpp           \
            qcan_server.hpp            \
            qcan_server_dialog.hpp
                
            
#---------------------------------------------------------------
# source files of project 
#
SOURCES =   qcan_interface_widget.cpp  \
            qcan_data.cpp              \
            qcan_frame.cpp             \
            qcan_frame_api.cpp         \
            qcan_frame_error.cpp       \
            qcan_timestamp.cpp         \
            qcan_network.cpp           \
            qcan_server.cpp            \
            qcan_server_dialog.cpp     \
            server_main.cpp

#---------------------------------------------------------------
# OS specific settings 
#
macx {

   CONFIG(debug, debug|release) {
      message("Building '$$QMAKE_PROJECT_NAME' DEBUG version for Mac OS X ...")
   } else {
      message("Building '$$QMAKE_PROJECT_NAME' RELEASE version for Mac OS X ...")
      DEFINES += QT_NO_WARNING_OUTPUT
      DEFINES += QT_NO_DEBUG_OUTPUT
   }

   #--------------------------------------------------
   # Use the supplied .plist file settings
   #
   QMAKE_INFO_PLIST = Info.plist
   
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
   ICON = ./images/server.icns
}

win32 {
   CONFIG(debug, debug|release) {
      message("Building '$$QMAKE_PROJECT_NAME' DEBUG version for Windows ...")
   } else {
      message("Building '$$QMAKE_PROJECT_NAME' RELEASE version for Windows ...")
      DEFINES += QT_NO_WARNING_OUTPUT
      DEFINES += QT_NO_DEBUG_OUTPUT
   }

   RC_FILE += server.rc
}
