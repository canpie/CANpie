TEMPLATE = app


QT += core network
QT += testlib

CONFIG += debug
CONFIG += silent
CONFIG += testcase

TARGET = qcantest

INCLUDEPATH += .
INCLUDEPATH += ../qcan
INCLUDEPATH += ../canpie/version3



DEFINES +=  

SOURCES +=  ../canpie/version3/canpie_frame.cpp       \
            ../canpie/version3/canpie_frame_api.cpp   \
            ../canpie/version3/canpie_frame_error.cpp \
            ../canpie/version3/canpie_timestamp.cpp   \
            ../qcan/qcan_frame.cpp        \
            ../qcan/qcan_frame_api.cpp    \
            ../qcan/qcan_frame_error.cpp  \
            ../qcan/qcan_socket.cpp       \
            ./test_main.cpp               \
            ./test_qcan_frame.cpp         \
            ./test_qcan_socket.cpp

FORMS   +=  


RESOURCES += 


HEADERS +=  ../qcan/qcan_frame.hpp        \
            ../qcan/qcan_interface.hpp    \
            ../qcan/qcan_socket.hpp       \
            ./test_qcan_frame.hpp         \
            ./test_qcan_socket.hpp