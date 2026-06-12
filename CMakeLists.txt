cmake_minimum_required(VERSION 3.16)

project(GateOfThree LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32 AND EXISTS "C:/SFML/lib/cmake/SFML")
    set(SFML_DIR "C:/SFML/lib/cmake/SFML" CACHE PATH "Path to SFML 2.x CMake config")
endif()

find_package(SFML 2 COMPONENTS audio graphics window system REQUIRED)

add_executable(GateOfThree
    src/main.cpp
    src/AssetPaths.cpp
    src/BattleRoom.cpp
    src/Boss.cpp
    src/BossRoom.cpp
    src/Chest.cpp
    src/ChestRoom.cpp
    src/Enemy.cpp
    src/Entity.cpp
    src/Game.cpp
    src/GameObject.cpp
    src/HealingSanctuary.cpp
    src/HealRoom.cpp
    src/Npc.cpp
    src/Mushroom.cpp
    src/Player.cpp
    src/Projectile.cpp
    src/RadialHealthIndicator.cpp
    src/Room.cpp
    src/RoomExit.cpp
    src/RoomManager.cpp
    src/Spell.cpp
    src/TutorialRoom.cpp
    include/AdditionalRoomMapData.h
    include/AssetPaths.h
    include/BattleRoom.h
    include/BattleRoomOneMapData.h
    include/Boss.h
    include/BossRoom.h
    include/Chest.h
    include/ChestRoom.h
    include/Enemy.h
    include/Entity.h
    include/Game.h
    include/GameObject.h
    include/HealingSanctuary.h
    include/HealRoom.h
    include/Npc.h
    include/Mushroom.h
    include/Player.h
    include/Projectile.h
    include/RadialHealthIndicator.h
    include/Room.h
    include/RoomExit.h
    include/RoomManager.h
    include/RoomType.h
    include/Spell.h
    include/TerrainCollision.h
    include/TutorialRoom.h
)

target_include_directories(GateOfThree PRIVATE include)
target_link_libraries(GateOfThree PRIVATE sfml-audio sfml-graphics sfml-window sfml-system)

string(REPLACE "\\" "/" ASSET_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Assets")
target_compile_definitions(GateOfThree PRIVATE ASSET_ROOT="${ASSET_ROOT_PATH}")

if(WIN32)
    set(SFML_BIN_DIR "C:/SFML/bin" CACHE PATH "Path to SFML 2.x DLL directory")
    if(EXISTS "${SFML_BIN_DIR}")
        file(GLOB SFML_RUNTIME_DLLS "${SFML_BIN_DIR}/*.dll")
        add_custom_command(TARGET GateOfThree POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${SFML_RUNTIME_DLLS}
                $<TARGET_FILE_DIR:GateOfThree>
        )
    endif()
endif()

add_custom_command(TARGET GateOfThree POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
        "$<TARGET_FILE_DIR:GateOfThree>/Assets"
)
