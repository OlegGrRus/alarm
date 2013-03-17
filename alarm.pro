#-------------------------------------------------
#
# Project created by QtCreator 2012-03-30T18:09:10
#
#-------------------------------------------------

QT       += core gui phonon

TARGET = Alarm
TEMPLATE = app


SOURCES += main.cpp\
        alarmclock.cpp \
    dialogaddalarm.cpp \
    messagedialog.cpp

HEADERS  += alarmclock.h \
    DialogAddAlarm.h \
    messagedialog.h

FORMS    += alarmclock.ui \
    dialogAddAlarm.ui \
    messagedialog.ui

RESOURCES += \
    myRes.qrc
