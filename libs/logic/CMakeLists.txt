cmake_minimum_required(VERSION 3.20)

# Supports standalone builds: cmake -S libs/logic -B build-logic
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(BombermanLogic LANGUAGES CXX)

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

add_library(bomberman_logic STATIC
    src/Stopwatch.cpp
    src/Random.cpp
    src/Camera.cpp
    src/Entity.cpp
    src/Character.cpp
    src/Bomb.cpp
    src/Wall.cpp
    src/PowerUp.cpp
    src/World.cpp
    src/Score.cpp
)

add_library(Bomberman::Logic ALIAS bomberman_logic)

target_include_directories(bomberman_logic
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(bomberman_logic PUBLIC cxx_std_20)

# Logic must never depend on SFML or other view-layer libraries.
target_compile_definitions(bomberman_logic
    PUBLIC
        BOMBERMAN_LOGIC_BUILD
)
