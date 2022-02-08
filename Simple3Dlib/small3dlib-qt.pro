#-------------------------------------------------
#
# Project created by QtCreator 2022-01-03T11:37:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = small3dlib-qt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    3dlib/chestModel.h \
    3dlib/chestTexture.h \
    3dlib/earthModel.h \
    3dlib/earthTexture.h \
    3dlib/gbmetaModel.h \
    3dlib/gbmetaTexture.h \
    3dlib/houseModel.h \
    3dlib/houseTexture.h \
    3dlib/modelViewer.hpp \
    3dlib/palette.h \
    3dlib/small3dlib.h \
    3dlib/chestTexture_hd256.bmp.h \
    3dlib/earth_256c.bmp.h \
    3dlib/houseTexture_256c.bmp.h \
    3dlib/meta_256c.bmp.h \
    3dlib/minimeta.h \
    Gamebuino-Meta.h \
    3dlib/car.obj.h \
    3dlib/cubetube.bmp.h \
    3dlib/cubetube.obj.h \
    3dlib/3dlib.ino \
    3dlib/logoprogrammez.bmp.h \
    3dlib/logoprogrammezModel.h

FORMS    += mainwindow.ui

INCLUDEPATH += "3dlib"

OTHER_FILES += \
    3dlib/ICON.BMP \
    3dlib/modelViewer.bin \
    3dlib/TITLESCREEN.BMP
