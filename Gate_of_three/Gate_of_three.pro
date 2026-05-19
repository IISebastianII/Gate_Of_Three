TEMPLATE = app
TARGET = Gate_of_three
DESTDIR = $$OUT_PWD/bin
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += \
    $$PWD/include \
    C:/SFML/include

DEFINES += ASSET_ROOT=\\\"$$PWD/Assets\\\"

SOURCES += \
    src/AssetPaths.cpp \
    src/Game.cpp \
    src/Player.cpp \
    src/StaticNpc.cpp \
    src/TutorialRoom.cpp \
    src/main.cpp

HEADERS += \
    include/AssetPaths.h \
    include/Game.h \
    include/Player.h \
    include/StaticNpc.h \
    include/TutorialRoom.h

CONFIG(debug, debug|release) {
    LIBS += -LC:/SFML/lib \
        -lsfml-graphics-d \
        -lsfml-window-d \
        -lsfml-system-d

    SFML_DLLS = \
        sfml-graphics-d-2.dll \
        sfml-window-d-2.dll \
        sfml-system-d-2.dll
} else {
    LIBS += -LC:/SFML/lib \
        -lsfml-graphics \
        -lsfml-window \
        -lsfml-system

    SFML_DLLS = \
        sfml-graphics-2.dll \
        sfml-window-2.dll \
        sfml-system-2.dll
}

win32 {
    SFML_BIN = C:/SFML/bin

    for(dll, SFML_DLLS) {
        QMAKE_POST_LINK += $$QMAKE_COPY $$shell_path($$SFML_BIN/$${dll}) $$shell_path($$DESTDIR/$${dll}) $$escape_expand(\n\t)
    }

    QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$shell_path($$PWD/Assets) $$shell_path($$DESTDIR/Assets) $$escape_expand(\n\t)
}
