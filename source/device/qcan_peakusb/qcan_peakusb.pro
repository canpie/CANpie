#! [0]
TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
INCLUDEPATH    += ../echowindow
INCLUDEPATH    += ./../../include
HEADERS         = qcan_peakusb.hpp
SOURCES         = qcan_peakusb.cpp
TARGET          = $$qtLibraryTarget(QCanPeakUsb)
DESTDIR         = ../../qcan/plugins
#! [0]

EXAMPLE_FILES = plugin.json


