cmake_minimum_required(VERSION 3.16)

project(GateOfThree LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32 AND EXISTS "C:/SFML/lib/cmake/SFML")
    set(SFML_DIR "C:/SFML/lib/cmake/SFML" CACHE PATH "Path to SFML 2.x CMake config")
endif()

find_package(SFML 2 COMPONENTS graphics window system REQUIRED)

add_executable(Gate_of_three
    src/main.cpp
    src/AssetPaths.cpp
    src/Game.cpp
    src/Player.cpp
    src/StaticNpc.cpp
    src/TutorialRoom.cpp
    include/AssetPaths.h
    include/Game.h
    include/Player.h
    include/StaticNpc.h
    include/TutorialRoom.h
)

target_include_directories(Gate_of_three PRIVATE include)
target_link_libraries(Gate_of_three PRIVATE sfml-graphics sfml-window sfml-system)

string(REPLACE "\\" "/" ASSET_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Assets")
target_compile_definitions(Gate_of_three PRIVATE ASSET_ROOT="${ASSET_ROOT_PATH}")

if(WIN32)
    set(SFML_BIN_DIR "C:/SFML/bin" CACHE PATH "Path to SFML 2.x DLL directory")
    if(EXISTS "${SFML_BIN_DIR}")
        file(GLOB SFML_RUNTIME_DLLS "${SFML_BIN_DIR}/*.dll")
        add_custom_command(TARGET Gate_of_three POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${SFML_RUNTIME_DLLS}
                $<TARGET_FILE_DIR:Gate_of_three>
        )
    endif()
endif()

add_custom_command(TARGET Gate_of_three POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
        "$<TARGET_FILE_DIR:Gate_of_three>/Assets"
)
