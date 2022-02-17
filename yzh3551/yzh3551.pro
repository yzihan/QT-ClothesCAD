#-------------------------------------------------
#
# Project created by QtCreator 2016-02-20T18:43:49
#
#-------------------------------------------------

QT       += core gui svg xml


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11


TARGET = yzh3551
TEMPLATE = app
CONFIG += warn_on

UI_DIR=./UI




SOURCES += main.cpp\
        mainwindow.cpp \
    handle.cpp \
    ellchange.cpp \
    pointmove.cpp \
    size.cpp \
    size.cpp \
    color.cpp \
    dots.cpp \
    text.cpp \
    baseitem.cpp \
    complexitem.cpp \
    ellipse.cpp \
    polyline.cpp \
    rectangle.cpp \
    selection.cpp \
    workplace.cpp \
    workplaceview.cpp

HEADERS  += mainwindow.h \
    handle.h \
    ui_mainwindow.h \
    ui_showhelpdialog.h \
    ui_textdialog.h \
    ui_veellipsesettings.h \
    ui_vepolylinesettings.h \
    ui_verectanglesettings.h \
    ui_showsvgdialog.h \
    move.h \
    ellchange.h \
    pointmove.h \
    size.h \
    size.h \
    color.h \
    dots.h \
    text.h \
    baseitem.h \
    complexitem.h \
    ellipse.h \
    polyline.h \
    rectangle.h \
    selection.h \
    workplace.h \
    workplaceview.h \
    complexitem.h

FORMS    += mainwindow.ui \
    ellchange.ui \
    pointmove.ui \
    size.ui \
    ellipse.ui \
    polyline.ui \
    rectangle.ui \
    text.ui

RESOURCES += \
    icons.qrc

OTHER_FILES += \
    ico.ico \
    VectorEditor_resource.rc \
    Makefile.Debug \
    Makefile.Release \
    object_script.VectorEditor.Debug \
    object_script.VectorEditor.Release \
    Makefile \
    VectorEditor_resource.rc \
    Makefile \
    Makefile.Debug \
    Makefile.Release \
    object_script.VectorEditor.Debug \
    object_script.VectorEditor.Release

DISTFILES += \
    test.qss
