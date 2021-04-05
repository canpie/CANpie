#=============================================================================#
# File:          can-error.pro                                                #
# Description:   qmake project file for can-error command                     #
#                                                                             #
# Copyright (C) MicroControl GmbH & Co. KG                                    #
# 53844 Troisdorf - Germany                                                   #
# www.microcontrol.net                                                        #
#                                                                             #
#=============================================================================#

#---------------------------------------------------------------
# Name of QMake project
#
QMAKE_PROJECT_NAME = "can-error"

#---------------------------------------------------------------
# template type
#
TEMPLATE = app

#---------------------------------------------------------------
# Qt modules used
#
QT += core network websockets

#---------------------------------------------------------------
# target file name
#
TARGET = can-error

#---------------------------------------------------------------
# Directory for target file
#
DESTDIR = ../../../bin

#---------------------------------------------------------------
# Directory for intermediate moc files
# 
MOC_DIR = ../../../objs

#--------------------------------------------------------------------
# Directory for object files
#
OBJECTS_DIR = ../../../objs


#---------------------------------------------------------------
# project configuration and compiler options
#
CONFIG += debug_and_release
CONFIG += warn_on
CONFIG += C++11
CONFIG += silent
CONFIG += console


#---------------------------------------------------------------
# version of the application
#
VERSION_MAJOR = 0
VERSION_MINOR = 99
VERSION_BUILD = 10


#---------------------------------------------------------------
# Target version
#
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}


#---------------------------------------------------------------
# definitions for preprocessor
#
DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
           "VERSION_MINOR=$$VERSION_MINOR"\
           "VERSION_BUILD=$$VERSION_BUILD"

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
INCLUDEPATH += ./../../qcan

#---------------------------------------------------------------
# search path for source files
#
VPATH  = .
VPATH += ./../../qcan

#---------------------------------------------------------------
# header files of project 
#
HEADERS =   qcan_socket.hpp            \
            qcan_error.hpp
                
            
#---------------------------------------------------------------
# source files of project 
#
SOURCES =   qcan_frame.cpp             \
            qcan_socket.cpp            \
            qcan_timestamp.cpp         \
            qcan_error.cpp
               
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
   # do not create application bundle
   #
   CONFIG -= app_bundle
   
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
   
}


unix:!macx{
   CONFIG(debug, debug|release) {
      message("Building '$$QMAKE_PROJECT_NAME' DEBUG version for Linux ...")
   } else {
      message("Building '$$QMAKE_PROJECT_NAME' RELEASE version for Linux ...")
      DEFINES += QT_NO_WARNING_OUTPUT
      DEFINES += QT_NO_DEBUG_OUTPUT
   }

   #--------------------------------------------------
   # Specify the install directory for the target
   # /usr/local/bin
   #
   target.path  = /usr/local/bin
   INSTALLS += target
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
