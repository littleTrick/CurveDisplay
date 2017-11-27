#-------------------------------------------------
#
# Project created by QtCreator 2017-11-09T19:13:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = curve_display
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    login.cpp \
    about.cpp \
    communication_property.cpp \
    curve_property.cpp \
    serialport.cpp \
    controller.cpp \
    data_collector.cpp \
    process_data_thread.cpp

HEADERS  += mainwindow.h \
    login.h \
    about.h \
    communication_property.h \
    curve_property.h \
    serialport.h \
    controller.h \
    data_collector.h \
    process_data_thread.h

FORMS    += mainwindow.ui \
    login.ui \
    curve_property.ui \
    about.ui \
    communication_property.ui

RESOURCES +=

#DEFINES += NDEBUG

INCLUDEPATH += /usr/local/qwt-6.1.4-svn/include
LIBS += -L"/usr/local/qwt-6.1.4-svn/lib/" -lqwt

#INCLUDEPATH += /opt/qwt-for-arm/qwt-6.1.4-svn/include
#LIBS += -L"/opt/qwt-for-arm/qwt-6.1.4-svn/lib/" -lqwt
