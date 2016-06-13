TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
DEFINES        +=
INCLUDEPATH    += ../echowindow
INCLUDEPATH    += ./../../qcan
INCLUDEPATH    += ./../../include
INCLUDEPATH    += ./inc
HEADERS         = qcan_ixxatusb.hpp \
                  ./../../qcan/qcan_frame.hpp
SOURCES         = qcan_ixxatusb.cpp \
                  ./../../qcan/qcan_frame.cpp
TARGET          = $$qtLibraryTarget(QCanIxxatUsb)
DESTDIR         = ../../qcan/plugins


EXAMPLE_FILES = plugin.json
