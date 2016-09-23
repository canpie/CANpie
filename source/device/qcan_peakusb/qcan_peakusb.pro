TARGET          = $$qtLibraryTarget(QCanPeakUsb)

TEMPLATE        = lib

CONFIG         += plugin
CONFIG         += silent

# Define different behaviour for debug and release builds
CONFIG += debug_and_release
win32 {
CONFIG(debug, debug|release) {
   QMAKE_POST_LINK += cp ../../qcan/plugins/*.dll ../../../source/build-qcan_server-Desktop_Qt_5_6_0_MinGW_32bit-Debug/plugins
} else {
   DEFINES += QT_NO_WARNING_OUTPUT
   DEFINES += QT_NO_DEBUG_OUTPUT
   QMAKE_POST_LINK += cp ../../qcan/plugins/*.dll ../../../source/build-qcan_server-Desktop_Qt_5_6_0_MinGW_32bit-Release/plugins
}
}

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

DESTDIR         = ../../qcan/plugins


RESOURCES      += qcan_peakusb.qrc

EXAMPLE_FILES = plugin.json


