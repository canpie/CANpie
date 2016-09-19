#! [0]
TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
INCLUDEPATH    += ../echowindow
INCLUDEPATH    += ./../../qcan
INCLUDEPATH    += ./../../include
HEADERS         = qcan_peakusb.hpp \
                  ./../../qcan/qcan_frame.hpp \
                  ./../../qcan/qcan_interafce.hpp
                  PCANBasic.h
SOURCES         = qcan_peakusb.cpp \
                  ./../../qcan/qcan_frame.cpp
TARGET          = $$qtLibraryTarget(QCanPeakUsb)
DESTDIR         = ../../qcan/plugins
RESOURCES      += qcan_peakusb.qrc
#! [0]


EXAMPLE_FILES = plugin.json


