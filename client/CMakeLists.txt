cmake_minimum_required(VERSION 3.15)
project(ChronoClash LANGUAGES CXX)

# Estándar C++17 obligatorio
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Verificar submódulo Raylib
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/raylib/CMakeLists.txt")
    message(FATAL_ERROR
        "El submódulo de Raylib no está inicializado.\n"
        "Ejecuta: git submodule update --init --recursive"
    )
endif()

# Configuración de Raylib para Emscripten
if(EMSCRIPTEN)
    set(PLATFORM "Web" CACHE STRING "" FORCE)
endif()

add_subdirectory(lib/raylib)
add_subdirectory(src)
