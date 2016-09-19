TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
DEFINES        +=
INCLUDEPATH    += ../echowindow
INCLUDEPATH    += ./../../qcan
INCLUDEPATH    += ./../../include
INCLUDEPATH    += ./inc
HEADERS         = qcan_ixxatusb.hpp \
                  ./../../qcan/qcan_frame.hpp \
                  ./../../qcan/qcan_interafce.hpp
SOURCES         = qcan_ixxatusb.cpp \
                  ./../../qcan/qcan_frame.cpp
TARGET          = $$qtLibraryTarget(QCanIxxatUsb)
DESTDIR         = ../../qcan/plugins
RESOURCES      += qcan_ixxatusb.qrc

EXAMPLE_FILES = plugin.json
