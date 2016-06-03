#! [0]
TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
INCLUDEPATH    += ../echowindow
INCLUDEPATH    += ./../../include
HEADERS         = qcan_ixxatusb.hpp
SOURCES         = qcan_ixxatusb.cpp
TARGET          = $$qtLibraryTarget(QCanIxxatUsb)
DESTDIR         = ../../qcan/plugins
#! [0]

EXAMPLE_FILES = plugin.json
