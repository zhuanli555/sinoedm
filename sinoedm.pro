#-------------------------------------------------
#
# Project created by QtCreator 2020-03-23T11:21:59
#
#-------------------------------------------------

QT       += core gui sql concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sinoedm
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    process.cpp \
    program.cpp \
    setting.cpp \
    EDM/EDM.cpp \
    EDM/EDM_Db.cpp \
    coordwidget.cpp \
    unionzero.cpp \
    codeeditor.cpp \
    axissetdialog.cpp \
    EDM/cmdhandle.cpp \
    alarmsignal.cpp \
    elecparatable.cpp \
    EDM/EDM_OP.cpp \
    EDM/EDM_OP_File.cpp \
    EDM/EDM_OP_HOLE.cpp \
    EDM/EDM_OP_List.cpp \
    EDM/electool.cpp \
    spindelegate.cpp \
#    elecoralmodel.cpp \
#    elecpagemodel.cpp

HEADERS += \
        mainwindow.h \
    process.h \
    program.h \
    setting.h \
    EDM/common.h \
    EDM/EDM.h \
    EDM/EDM_Db.h \
    EDM/initdb.h \
    coordwidget.h \
    unionzero.h \
    codeeditor.h \
    axissetdialog.h \
    EDM/cmdhandle.h \
    alarmsignal.h \
    elecparatable.h \
    EDM/EDM_OP.h \
    EDM/EDM_OP_File.h \
    EDM/EDM_OP_HOLE.h \
    EDM/EDM_OP_List.h \
    EDM/electool.h \
    spindelegate.h \
#    //elecoralmodel.h \
#    //elecpagemodel.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
