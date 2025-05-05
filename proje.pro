QT += core gui widgets

CONFIG += c++17


CONFIG += debug

QT += multimedia
QT += svg

SOURCES += \
    arenaselectionscreen.cpp \
    galleryscreen.cpp \
    main.cpp \
    menuscreen.cpp \
    playernamescreen.cpp \
    splashscreen.cpp \
    gameboardscreen.cpp

HEADERS += \
    arenaselectionscreen.h \
    galleryscreen.h \
    menuscreen.h \
    playernamescreen.h \
    splashscreen.h \
    gameboardscreen.h

RESOURCES += \
    resources.qrc


QMAKE_CXXFLAGS += -std=c++17


QMAKE_CXXFLAGS += -Wall -Wextra


target.path = $$[QT_INSTALL_EXAMPLES]/games/yourgame
INSTALLS += target
