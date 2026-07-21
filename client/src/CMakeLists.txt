# Registrar módulos
add_subdirectory(game)
add_subdirectory(net)
add_subdirectory(ui)
add_subdirectory(utils)

# Target principal
add_executable(ChronoClash main.cpp)

target_link_libraries(ChronoClash PRIVATE
    game
    net
    ui
    utils
    raylib
)

# Configuración específica de Emscripten
if(EMSCRIPTEN)
    set_target_properties(ChronoClash PROPERTIES
        SUFFIX ".html"
        OUTPUT_NAME "index"
    )

    target_link_options(ChronoClash PRIVATE
        "-sUSE_GLFW=3"
        "-sASYNCIFY"
        "-sALLOW_MEMORY_GROWTH=1"
        "-lwebsocket.js"
        "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/../assets@/assets"
        "--shell-file=${CMAKE_CURRENT_SOURCE_DIR}/shell.html"
    )

    # Flags Debug específicos de Emscripten
    target_link_options(ChronoClash PRIVATE
        "$<$<CONFIG:Debug>:-sASSERTIONS=1>"
        "$<$<CONFIG:Debug>:-gsource-map>"
        "$<$<CONFIG:Release>:--closure=1>"
    )
endif()

# Warnings para código del proyecto
target_compile_options(ChronoClash PRIVATE -Wall -Wextra)

# Flags de optimización por configuración
target_compile_options(ChronoClash PRIVATE
    "$<$<CONFIG:Debug>:-O0;-g>"
    "$<$<CONFIG:Release>:-O2>"
)
