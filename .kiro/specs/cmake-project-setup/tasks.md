# Plan de Implementación: cmake-project-setup

## Resumen

Migrar el sistema de build del cliente ChronoClash de Makefile a CMake. Se crearán los archivos CMakeLists.txt siguiendo la jerarquía modular descrita en el diseño: raíz → src → módulos (game, net, ui, utils), con integración de Raylib y configuración de Emscripten para producir artefactos WebAssembly.

## Tareas

- [x] 1. Crear el CMakeLists.txt raíz y la integración de Raylib
  - [x] 1.1 Crear client/CMakeLists.txt con la configuración global del proyecto
    - Definir `cmake_minimum_required(VERSION 3.15)` y `project(ChronoClash LANGUAGES CXX)`
    - Configurar C++17 como estándar obligatorio (`CMAKE_CXX_STANDARD 17`, `CMAKE_CXX_STANDARD_REQUIRED ON`, `CMAKE_CXX_EXTENSIONS OFF`)
    - Agregar verificación de submódulo Raylib con `FATAL_ERROR` si `lib/raylib/CMakeLists.txt` no existe
    - Configurar `PLATFORM "Web"` condicional a `EMSCRIPTEN`
    - Invocar `add_subdirectory(lib/raylib)` y `add_subdirectory(src)`
    - _Requisitos: 1.1, 3.1, 3.2, 3.4, 5.5_

- [x] 2. Crear los CMakeLists.txt de módulos
  - [x] 2.1 Crear client/src/utils/CMakeLists.txt
    - Definir la biblioteca (INTERFACE o STATIC según contenido actual del módulo)
    - Configurar `target_include_directories` con visibilidad PUBLIC/INTERFACE
    - _Requisitos: 1.2, 6.1, 6.4_

  - [x] 2.2 Crear client/src/game/CMakeLists.txt
    - Definir biblioteca estática `game` con todos los archivos fuente listados (game.cpp, player.cpp, ability.cpp, level.cpp, hazards.cpp, camera.cpp)
    - Configurar `target_include_directories(game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})`
    - Declarar dependencia `target_link_libraries(game PUBLIC utils)`
    - Agregar `target_compile_options(game PRIVATE -Wall -Wextra)`
    - _Requisitos: 1.2, 6.1, 6.4, 7.3_

  - [x] 2.3 Crear client/src/net/CMakeLists.txt
    - Definir biblioteca estática `net` con archivos fuente (net.cpp, messages.cpp)
    - Configurar `target_include_directories(net PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})`
    - Declarar dependencia `target_link_libraries(net PUBLIC utils)`
    - Agregar `target_compile_options(net PRIVATE -Wall -Wextra)`
    - _Requisitos: 1.2, 6.1, 6.4, 7.3_

  - [x] 2.4 Crear client/src/ui/CMakeLists.txt
    - Definir biblioteca estática `ui` con archivos fuente (ui.cpp, screens.cpp)
    - Configurar `target_include_directories(ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})`
    - Declarar dependencia `target_link_libraries(ui PUBLIC game)`
    - Agregar `target_compile_options(ui PRIVATE -Wall -Wextra)`
    - _Requisitos: 1.2, 6.1, 6.4, 7.3_

- [x] 3. Crear el CMakeLists.txt de src/ con el target principal
  - [x] 3.1 Crear client/src/CMakeLists.txt con registro de módulos y target ejecutable
    - Registrar los 4 módulos con `add_subdirectory(game)`, `add_subdirectory(net)`, `add_subdirectory(ui)`, `add_subdirectory(utils)`
    - Definir `add_executable(ChronoClash main.cpp)`
    - Enlazar módulos y raylib con `target_link_libraries(ChronoClash PRIVATE game net ui utils raylib)`
    - _Requisitos: 1.3, 2.1, 2.2, 2.3, 3.3_

  - [x] 3.2 Configurar flags de Emscripten en el target principal
    - Dentro de bloque `if(EMSCRIPTEN)`: establecer `SUFFIX ".html"` y `OUTPUT_NAME "index"`
    - Configurar `target_link_options` con `-sUSE_GLFW=3`, `-sASYNCIFY`, `-sALLOW_MEMORY_GROWTH=1`, `-lwebsocket.js`
    - Configurar `--preload-file` para assets y `--shell-file` para shell.html
    - Agregar flags condicionales Debug/Release con generator expressions (`-sASSERTIONS=1`, `-gsource-map`, `--closure=1`)
    - _Requisitos: 2.4, 2.5, 4.1, 4.2, 4.3, 4.4, 4.5, 7.1, 7.2_

  - [x] 3.3 Configurar warnings y flags de optimización del target principal
    - Agregar `target_compile_options(ChronoClash PRIVATE -Wall -Wextra)`
    - Agregar flags de optimización por configuración: `-O0 -g` en Debug, `-O2` en Release
    - _Requisitos: 7.1, 7.2, 7.3, 7.4_

- [x] 4. Checkpoint - Verificar estructura CMake
  - Asegurar que todos los archivos CMakeLists.txt están creados y contienen las directivas esperadas, preguntar al usuario si surgen dudas.

- [x] 5. Verificación y pruebas de humo
  - [x] 5.1 Crear script de verificación de estructura CMake
    - Crear un script (bash o CMake) que verifique la existencia de todos los CMakeLists.txt esperados
    - Verificar que las directivas clave están presentes en cada archivo (cmake_minimum_required, add_library, target_link_libraries, flags de Emscripten)
    - _Requisitos: 1.1, 1.2, 1.3, 5.1, 5.2_

  - [x] 5.2 Escribir tests de integración para el flujo de build
    - Verificar que `cmake -B build -DCMAKE_TOOLCHAIN_FILE=...` seguido de `cmake --build build` retorna código 0
    - Verificar que los artefactos index.html, index.js, index.wasm, index.data se generan con tamaño > 0
    - _Requisitos: 5.1, 5.2, 5.3, 5.4_

  - [x] 5.3 Escribir tests de condiciones de error
    - Verificar que sin submódulo Raylib inicializado, cmake falla con FATAL_ERROR y mensaje instructivo
    - Verificar que con toolchain inválido, cmake falla con código != 0
    - _Requisitos: 3.4, 5.3, 5.4_

- [x] 6. Checkpoint final - Validación completa
  - Asegurar que todos los archivos están correctos y la estructura es coherente con el diseño, preguntar al usuario si surgen dudas.

## Notas

- Las tareas marcadas con `*` son opcionales y pueden omitirse para un MVP más rápido
- Cada tarea referencia requisitos específicos para trazabilidad
- Los checkpoints aseguran validación incremental
- No se incluyen property-based tests porque el diseño indica que no son aplicables para esta feature de infraestructura de build
- Los tests de integración (5.2, 5.3) requieren Emscripten SDK instalado en el entorno
- El módulo `utils` puede ser INTERFACE (header-only) o STATIC dependiendo de si tiene archivos .cpp; revisar el estado actual del directorio al implementar

## Task Dependency Graph

```json
{
  "waves": [
    { "id": 0, "tasks": ["1.1", "2.1"] },
    { "id": 1, "tasks": ["2.2", "2.3", "2.4"] },
    { "id": 2, "tasks": ["3.1"] },
    { "id": 3, "tasks": ["3.2", "3.3"] },
    { "id": 4, "tasks": ["5.1"] },
    { "id": 5, "tasks": ["5.2", "5.3"] }
  ]
}
```
