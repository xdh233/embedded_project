QT += core gui sql
TARGET = SmartHomeMonitor
TEMPLATE = app

CONFIG += qt warn_on release
CONFIG += thread

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    sensorthread.cpp \
    chartwidget.cpp \

HEADERS += \
    mainwindow.h \
    sensorthread.h \
    chartwidget.h \
    sensordata.h

INCLUDEPATH += .

# 嵌入式优化
#QMAKE_CXXFLAGS += -O2 -march=armv4t -mtune=arm920t
#DEFINES += QT_NO_DEBUG_OUTPUT

target.path = /opt/smarthome
INSTALLS += target
