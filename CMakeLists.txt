cmake_minimum_required(VERSION 3.20)

project(Bomberman
    VERSION 0.1.0
    DESCRIPTION "Bomberman game with strict MVC separation"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE/clang-tidy integration.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------------------
# Architecture
#   libs/logic  — Model + game rules (no SFML, no rendering)
#   src/view    — View + Controller executable (SFML, links Bomberman::Logic)
# ---------------------------------------------------------------------------

add_subdirectory(libs/logic)
add_subdirectory(src/view)
