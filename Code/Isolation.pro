QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Isolation
TEMPLATE = app

CONFIG += c++14

SOURCES += \
    animation.cpp \
    framebuffer.cpp \
    main.cpp \
    mainwindow.cpp \
    mainview.cpp \
    mesh.cpp \
    scene.cpp \
    shader.cpp \
    texture.cpp \
    transform.cpp \
    user_input.cpp \
    model.cpp

HEADERS += \
    animation.h \
    framebuffer.h \
    light.h \
    mainwindow.h \
    mainview.h \
    material.h \
    mesh.h \
    model.h \
    scene.h \
    shader.h \
    texture.h \
    transform.h \
    vertex.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources.qrc
