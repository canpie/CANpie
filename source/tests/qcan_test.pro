TEMPLATE = app


QT += core network
QT += testlib

CONFIG += debug
CONFIG += silent
CONFIG += testcase

TARGET = qcantest

INCLUDEPATH += .
INCLUDEPATH += ../qcan


DEFINES +=  

SOURCES +=  ../qcan/qcan_client.cpp       \
            ../qcan/qcan_frame.cpp        \
            ../qcan/qcan_frame_error.cpp  \
            ../qcan/qcan_network.cpp      \
            ../qcan/qcan_socket.cpp         \
            ./test_main.cpp            \
            ./test_qcan_client.cpp     \
            ./test_qcan_frame.cpp

FORMS   +=  


RESOURCES += 


HEADERS +=  ../qcan/qcan_client.hpp       \
            ../qcan/qcan_frame.hpp        \
            ../qcan/qcan_frame_error.hpp  \
            ../qcan/qcan_interface.hpp    \
            ../qcan/qcan_network.hpp      \
            ../qcan/qcan_socket.hpp         \
            ./test_qcan_client.hpp        \
            ./test_qcan_frame.hpp