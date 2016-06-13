#! [0]
TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
INCLUDEPATH    += ../echowindow
INCLUDEPATH    += ./../../qcan
INCLUDEPATH    += ./../../include
HEADERS         = qcan_peakusb.hpp \
                  ./../../qcan/qcan_frame.hpp \
                  PCANBasic.h
SOURCES         = qcan_peakusb.cpp \
                  ./../../qcan/qcan_frame.cpp
TARGET          = $$qtLibraryTarget(QCanPeakUsb)
DESTDIR         = ../../qcan/plugins
#! [0]

EXAMPLE_FILES = plugin.json


