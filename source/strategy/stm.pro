#-------------------------------------------------
#
# Project created by QtCreator 2012-03-28T08:13:08
#
#-------------------------------------------------

QT += network
QT +=thread
QT += opengl
LIBS += -lprotobuf
LIBS += .\Libs\WSock32.lib

QT       += core gui

TARGET = stm
TEMPLATE = app


SOURCES += main.cpp\
        frame.cpp \
    strategy.cpp \
    Robot.cpp \
    gui.cpp \
    waypoint.cpp \
    syncdata.cpp \
    ProtoBuf/strategy.pb.cc \
    PracticalSocket.cpp

HEADERS  += frame.h \
    strategy.h \
    Robot.h \
    gui.h \
    waypoint.h \
    syncdata.h \
    ProtoBuf/strategy.pb.h \
    PracticalSocket.h

FORMS    += frame.ui
