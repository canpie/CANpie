#! [0]
TEMPLATE     = lib
CONFIG      += plugin
QT          += widgets
DEFINES     +=
INCLUDEPATH += .
INCLUDEPATH += ./../include
HEADERS     += ./qcan_client.hpp \
               ./qcan_frame.hpp  \
               ./qcan_interface.hpp
SOURCES     += ./qcan_client.cpp \
               ./qcan_frame.cpp
FORMS       +=
RESOURCES   +=
TARGET       = $$qtLibraryTarget(QCanClient)
DESTDIR      = ../qcan/client
#! [0]

EXAMPLE_FILES = plugin.json
