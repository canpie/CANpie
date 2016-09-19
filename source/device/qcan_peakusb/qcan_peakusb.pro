TEMPLATE        = lib

CONFIG         += plugin
CONFIG         += silent

QT             += widgets

INCLUDEPATH    += ./../../qcan
INCLUDEPATH    += ./../../include

HEADERS         = qcan_peakusb.hpp                    \
                  ./../../qcan/qcan_frame.hpp         \
                  ./../../qcan/qcan_frame_error.hpp   \
                  ./../../qcan/qcan_interface.hpp
                  
SOURCES         = qcan_peakusb.cpp                    \
                  ./../../qcan/qcan_frame.cpp         \
                  ./../../qcan/qcan_frame_error.cpp

TARGET          = $$qtLibraryTarget(QCanPeakUsb)

DESTDIR         = ../../qcan/plugins

RESOURCES      += qcan_peakusb.qrc


EXAMPLE_FILES = plugin.json


