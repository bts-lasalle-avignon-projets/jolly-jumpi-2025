QT       += core gui bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    communication.cpp \
    baseDeDonnees.cpp \
    ihm.cpp \
    joueur.cpp \
    tir.cpp \
    statistiques.cpp \
    gestionPartie.cpp \
    main.cpp

HEADERS += \
    communication.h \
    baseDeDonnees.h \
    ihm.h \
    joueur.h \
    tir.h \
    statistiques.h \
    gestionPartie.h

# Les defines pour la version release (sans debug)
#CONFIG(release, debug|release):DEFINES+=QT_NO_DEBUG_OUTPUT RPI
# Les defines pour la version debug
CONFIG(debug, debug|release):DEFINES+=DEBUG

DISTFILES +=

FORMS += \
    accueil.ui \
    classement.ui \
    partie.ui
