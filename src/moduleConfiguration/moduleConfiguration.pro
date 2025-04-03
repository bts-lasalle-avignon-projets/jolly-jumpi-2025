QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
QT += bluetooth

DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    communication.cpp \
    gestionPartie.cpp \
    ihmmoduleconfiguration.cpp \
    main.cpp

HEADERS += \
    communication.h \
    gestionPartie.h \
    ihmmoduleconfiguration.h

FORMS += \
    ihmmoduleconfiguration.ui

# Les defines pour la version release (sans debug)
#CONFIG(release, debug|release):DEFINES+=QT_NO_DEBUG_OUTPUT RASPBERRY_PI
# Les defines pour la version debug
CONFIG(debug, debug|release):DEFINES+=
