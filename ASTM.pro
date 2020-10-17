#-------------------------------------------------
#
# Project created by QtCreator 2018-07-25T07:28:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ASTM
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


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    zodiacgraph/baseedge.cpp \
    zodiacgraph/bezieredge.cpp \
    zodiacgraph/drawedge.cpp \
    zodiacgraph/edgearrow.cpp \
    zodiacgraph/edgegroup.cpp \
    zodiacgraph/edgegroupinterface.cpp \
    zodiacgraph/edgegrouppair.cpp \
    zodiacgraph/edgelabel.cpp \
    zodiacgraph/labeltextfactory.cpp \
    zodiacgraph/node.cpp \
    zodiacgraph/nodehandle.cpp \
    zodiacgraph/nodelabel.cpp \
    zodiacgraph/perimeter.cpp \
    zodiacgraph/plug.cpp \
    zodiacgraph/plugarranger.cpp \
    zodiacgraph/plugedge.cpp \
    zodiacgraph/plughandle.cpp \
    zodiacgraph/pluglabel.cpp \
    zodiacgraph/scene.cpp \
    zodiacgraph/scenehandle.cpp \
    zodiacgraph/straightdoubleedge.cpp \
    zodiacgraph/straightedge.cpp \
    zodiacgraph/view.cpp \
    collapsible.cpp \
    mainctrl.cpp \
    nodectrl.cpp \
    nodeproperties.cpp \
    propertyeditor.cpp

HEADERS += \
        mainwindow.h \
    zodiacgraph/baseedge.h \
    zodiacgraph/bezieredge.h \
    zodiacgraph/drawedge.h \
    zodiacgraph/edgearrow.h \
    zodiacgraph/edgegroup.h \
    zodiacgraph/edgegroupinterface.h \
    zodiacgraph/edgegrouppair.h \
    zodiacgraph/edgelabel.h \
    zodiacgraph/labeltextfactory.h \
    zodiacgraph/node.h \
    zodiacgraph/nodehandle.h \
    zodiacgraph/nodelabel.h \
    zodiacgraph/perimeter.h \
    zodiacgraph/plug.h \
    zodiacgraph/plugarranger.h \
    zodiacgraph/plugedge.h \
    zodiacgraph/plughandle.h \
    zodiacgraph/pluglabel.h \
    zodiacgraph/scene.h \
    zodiacgraph/scenehandle.h \
    zodiacgraph/straightdoubleedge.h \
    zodiacgraph/straightedge.h \
    zodiacgraph/utils.h \
    zodiacgraph/view.h \
    collapsible.h \
    mainctrl.h \
    nodectrl.h \
    nodeproperties.h \
    propertyeditor.h

RESOURCES += \
    res/icons.qrc
