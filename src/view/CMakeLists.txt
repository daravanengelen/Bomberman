# Reference platform (university labs): Ubuntu 24.04, SFML 2.6.1, CMake 3.28.3
find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)

add_executable(bomberman
    main.cpp
)

target_link_libraries(bomberman
    PRIVATE
        Bomberman::Logic
        sfml-graphics
        sfml-window
        sfml-system
)

target_compile_features(bomberman PRIVATE cxx_std_20)

if(APPLE)
    set_target_properties(bomberman PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()
