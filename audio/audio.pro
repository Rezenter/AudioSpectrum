#-------------------------------------------------
#
# Project created by QtCreator 2017-11-20T07:41:42
#
#-------------------------------------------------

QT       += core gui multimedia charts serialport


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AudioSubdirs
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    audiorecorder.cpp \
    spectrumanalyser.cpp \
    frequencyspectrum.cpp \
    utils.cpp \
    comchatter.cpp \
    spectrograph.cpp

HEADERS += \
    mainwindow.h \
    audiorecorder.h \
    frequencyspectrum.h \
    spectrum.h \
    spectrumanalyser.h \
    utils.h \
    comchatter.h \
    spectrograph.h

FORMS += \
    mainwindow.ui

DEFINES +=\
    NDEBUG


LIBS += -L$$OUT_PWD/../3rdparty/fftreal/-lfftreal

INCLUDEPATH += $$PWD/../3rdparty/fftreal
DEPENDPATH += $$PWD/../3rdparty/fftreal

LIBS += -L..$${AudioSubdirs_build_dir}/lib
LIBS += -lfftreal
