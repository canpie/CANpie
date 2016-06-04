TEMPLATE = app


QT += core network
QT += testlib

CONFIG += debug
CONFIG += silent

TARGET = qcanclient

INCLUDEPATH += .
INCLUDEPATH += ./../include


DEFINES +=  

SOURCES +=  ./qcan_client.cpp \
            ./qcan_frame.cpp  \
            ./main_canpie.c 

FORMS   +=  


RESOURCES += 


HEADERS +=  ./qcan_client.hpp \
            ./qcan_frame.hpp  \
            ./qcan_interface.hpp