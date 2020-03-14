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
QT += core gui network websockets widgets

#---------------------------------------------------------------
# target file name
#
TARGET = CANpieServer

#---------------------------------------------------------------
# directory for target file
#
DESTDIR = ../../../bin

# Do not use a common /objs folder together with MSVC compiler.
# If more than one application is compiled from qcan with MSVC
# and common object folder is used, this can lead to linking
# errors because some object files build with different settings
# are mixed. This occurs especially when one application is
# compiled as a release and others as a debug version.
!win32-msvc* {
MOC_DIR = ../../../build
RCC_DIR = ../../../build
}
else {
#--------------------------------------------------------------------
# Objects directory
#
OBJECTS_DIR = ../../../build
}

#---------------------------------------------------------------
# project configuration and compiler options
#
CONFIG += debug_and_release
CONFIG += warn_on
CONFIG += C++11
CONFIG += silent


#---------------------------------------------------------------
# version of the application
#
VERSION_MAJOR = 0
VERSION_MINOR = 99
VERSION_BUILD = 06


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
FORMS   =  ./forms/ServerConfig.ui


#---------------------------------------------------------------
# Translation files and commands for building .qm files
#
TRANSLATIONS = $$files($$PWD/translations/*.ts)

QMAKE_EXTRA_COMPILERS += lrelease
lrelease.input         = TRANSLATIONS
lrelease.output        = $$PWD/translations/${QMAKE_FILE_BASE}.qm
lrelease.commands      = $$[QT_INSTALL_BINS]/lrelease ${QMAKE_FILE_IN} -qm $$PWD/translations/${QMAKE_FILE_BASE}.qm
lrelease.CONFIG       += no_link target_predeps

#---------------------------------------------------------------
# resource collection files 
#
RESOURCES = server.qrc


#---------------------------------------------------------------
# include directory search path
#
INCLUDEPATH  = .
INCLUDEPATH += ./../../qcan
INCLUDEPATH += ./../../canpie-fd

#---------------------------------------------------------------
# search path for source files
#
VPATH  = .
VPATH += ./../../qcan


#---------------------------------------------------------------
# Common header files of project 
#
HEADERS =   qcan_defs.hpp               \
            qcan_frame.hpp              \
            qcan_interface.hpp         	\
            qcan_network.hpp           	\
            qcan_server.hpp            	\
            qcan_server_logger.hpp     	\
            qcan_server_logger_view.hpp

#---------------------------------------------------------------
# Specific header files of project 
#
HEADERS +=  qcan_interface_widget.hpp  	\
            qcan_server_dialog.hpp
                        
#---------------------------------------------------------------
# Common source files of project 
#
SOURCES =   qcan_frame.cpp             	\
            qcan_network.cpp           	\
            qcan_server.cpp            	\
            qcan_server_logger.cpp     	\
            qcan_server_logger_view.cpp   \
            qcan_timestamp.cpp

#---------------------------------------------------------------
# Specific source files of project 
#
SOURCES +=  qcan_interface_widget.cpp  	\
            qcan_server_dialog.cpp     	\
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
	# Add App Nap suspender class for Mac OS only
	#
   SOURCES += MacAppNapSuspender.mm
   
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
   ICON = ./images/canpie_server.icns
   
   #--------------------------------------------------
	# Use Foundation framework for App Nap suspender 
	#
   LIBS += -framework Foundation
   
   #--------------------------------------------------
   # Specify the install directory for the target
   # /usr/local/bin
   #
   target.files = $$DESTDIR/$$TARGET.app/*
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

   RC_FILE += server.rc
}
