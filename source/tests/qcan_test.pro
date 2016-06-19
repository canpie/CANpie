TEMPLATE = app


QT += core network
QT += testlib

CONFIG += debug
CONFIG += silent
CONFIG += testcase

TARGET = qcantest

INCLUDEPATH += .
INCLUDEPATH += ../include
INCLUDEPATH += ../qcan



DEFINES +=  

SOURCES +=  ../qcan/qcan_frame.cpp        \
            ../qcan/qcan_frame_api.cpp    \
            ../qcan/qcan_frame_error.cpp  \
            ../qcan/qcan_network.cpp      \
            ../qcan/qcan_stub.cpp         \
            ../qcan/qcan_stub_canpie.cpp  \
            ./test_main.cpp               \
            ./test_qcan_client.cpp        \
            ./test_qcan_frame.cpp         \
            ./test_qcan_stub.cpp

FORMS   +=  


RESOURCES += 


HEADERS +=  ../qcan/qcan_frame.hpp        \
            ../qcan/qcan_interface.hpp    \
            ../qcan/qcan_network.hpp      \
            ./test_qcan_client.hpp        \
            ./test_qcan_frame.hpp         \
            ./test_qcan_stub.hpp