add_library(bomberman_logic STATIC
    src/Stopwatch.cpp
    src/Random.cpp
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
