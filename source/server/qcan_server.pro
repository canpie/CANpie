TEMPLATE = app


QT += core network widgets
QT += testlib

CONFIG += debug
CONFIG += silent

TARGET = qcanserver

INCLUDEPATH += .
INCLUDEPATH += ../qcan
INCLUDEPATH += ../include



DEFINES +=  

SOURCES +=  ../qcan/qcan_frame.cpp        \
            ../qcan/qcan_frame_api.cpp    \
            ../qcan/qcan_frame_error.cpp  \
            ../qcan/qcan_network.cpp      \
            ../qcan/qcan_server.cpp       \
            ./qcan_server_dialog.cpp      \
            ./server_main.cpp


FORMS   +=  ./forms/ServerConfig.ui


RESOURCES += qcanserver.qrc


HEADERS +=  ../qcan/qcan_frame.hpp        \
            ../qcan/qcan_interface.hpp    \
            ../qcan/qcan_network.hpp      \
            ../qcan/qcan_server.hpp       \
            ./qcan_server_dialog.hpp            

macx {
   message("Building for Mac OS X ...")
   QMAKE_INFO_PLIST = Info.plist
   ICON = ./images/qcanserver.icns
}
            