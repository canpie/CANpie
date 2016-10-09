#! [0]
QT += widgets
INCLUDEPATH += .
INCLUDEPATH += ./..
INCLUDEPATH += ./../qcan
INCLUDEPATH += ./../../include
HEADERS      = ./../qcan_interface.hpp \
               ./../qcan_frame.hpp \
               ./qcan_gui.hpp
SOURCES      = main.cpp \
               qcan_gui.cpp \
               ./../qcan_frame.cpp
TARGET       = qcan_gui
QMAKE_PROJECT_NAME = qcan_gui

win32 {
    CONFIG(debug, release|debug):DESTDIR = ../debug/
    CONFIG(release, release|debug):DESTDIR = ../release/
} else {
    DESTDIR    = ../
}
#! [0]
