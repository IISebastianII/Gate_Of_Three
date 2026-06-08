TEMPLATE = app
TARGET = GateOfThree
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
    src/BattleRoom.cpp \
    src/Boss.cpp \
    src/BossRoom.cpp \
    src/Chest.cpp \
    src/ChestRoom.cpp \
    src/Enemy.cpp \
    src/Entity.cpp \
    src/Game.cpp \
    src/GameObject.cpp \
    src/HealRoom.cpp \
    src/Npc.cpp \
    src/Player.cpp \
    src/Projectile.cpp \
    src/RadialHealthIndicator.cpp \
    src/Room.cpp \
    src/RoomExit.cpp \
    src/RoomManager.cpp \
    src/Spell.cpp \
    src/TutorialRoom.cpp \
    src/main.cpp

HEADERS += \
    include/AssetPaths.h \
    include/BattleRoom.h \
    include/Boss.h \
    include/BossRoom.h \
    include/Chest.h \
    include/ChestRoom.h \
    include/Enemy.h \
    include/Entity.h \
    include/Game.h \
    include/GameObject.h \
    include/HealRoom.h \
    include/Npc.h \
    include/Player.h \
    include/Projectile.h \
    include/RadialHealthIndicator.h \
    include/Room.h \
    include/RoomExit.h \
    include/RoomManager.h \
    include/RoomType.h \
    include/Spell.h \
    include/TutorialRoom.h

OTHER_FILES += \
    .gitignore \
    CMakeLists.txt \
    README.md \
    $$files($$PWD/Assets/*, true)

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
