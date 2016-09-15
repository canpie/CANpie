TEMPLATE = app


QT += core gui network widgets


CONFIG += debug
CONFIG += silent

TARGET = qcanclient

INCLUDEPATH += .
INCLUDEPATH += ../qcan
INCLUDEPATH += ../include

DEFINES +=  "MC_TARGET=MC_OS_LINUX"

FORMS   +=  ./forms/ClientDemo.ui

SOURCES +=  ./client_demo.cpp \
            ./client_main.cpp \
            ../qcan/qcan_frame.cpp        \
            ../qcan/qcan_frame_api.cpp    \
            ../qcan/qcan_frame_error.cpp  \
            ../qcan/qcan_socket.cpp
            
HEADERS +=  ./client_demo.hpp \
            ../qcan/qcan_frame.hpp        \
            ../qcan/qcan_interface.hpp    \
            ../qcan/qcan_socket.hpp

RESOURCES += 


