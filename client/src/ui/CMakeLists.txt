add_library(ui STATIC
    ui.cpp
    screens.cpp
)

target_include_directories(ui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ui PUBLIC
    raylib
    game
)

target_compile_options(ui PRIVATE -Wall -Wextra)
