TEMPLATE = app


QT += core network
QT += testlib

CONFIG += debug
CONFIG += silent

TARGET = qcanclient

INCLUDEPATH += .
INCLUDEPATH += ./../include
INCLUDEPATH += ./../qcan



DEFINES +=  

SOURCES +=  ./qcan_stub.cpp \
            ./qcan_frame.cpp  \
            ./main_canpie.cpp

FORMS   +=  


RESOURCES += 


HEADERS +=  ./qcan_stub.hpp \
            ./qcan_frame.hpp  \
            ./qcan_interface.hpp