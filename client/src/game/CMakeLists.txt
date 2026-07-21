add_library(game STATIC
    game.cpp
    player.cpp
    ability.cpp
    level.cpp
    hazards.cpp
    camera.cpp
)

target_include_directories(game PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(game PUBLIC
    utils
)

# Warnings para el módulo
target_compile_options(game PRIVATE -Wall -Wextra)
