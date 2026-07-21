# ==============================================================================
# Script de verificación de estructura CMake para ChronoClash
# Uso: cmake -P client/tests/verify_structure.cmake
# Ejecutar desde la raíz del repositorio (hackathon-kiro-AI/)
# ==============================================================================

set(PASS_COUNT 0)
set(FAIL_COUNT 0)
set(BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/..")

# ------------------------------------------------------------------------------
# Función: check_file_exists
# Verifica que un archivo existe en la ruta indicada
# ------------------------------------------------------------------------------
macro(check_file_exists FILE_PATH DESCRIPTION)
    if(EXISTS "${FILE_PATH}")
        message(STATUS "[PASS] ${DESCRIPTION} existe")
        math(EXPR PASS_COUNT "${PASS_COUNT} + 1")
    else()
        message(STATUS "[FAIL] ${DESCRIPTION} NO existe: ${FILE_PATH}")
        math(EXPR FAIL_COUNT "${FAIL_COUNT} + 1")
    endif()
endmacro()

# ------------------------------------------------------------------------------
# Función: check_directive
# Verifica que una directiva/cadena está presente en el contenido de un archivo
# ------------------------------------------------------------------------------
macro(check_directive FILE_CONTENT DIRECTIVE FILE_NAME)
    string(FIND "${FILE_CONTENT}" "${DIRECTIVE}" _pos)
    if(_pos GREATER -1)
        message(STATUS "[PASS] ${FILE_NAME} contiene '${DIRECTIVE}'")
        math(EXPR PASS_COUNT "${PASS_COUNT} + 1")
    else()
        message(STATUS "[FAIL] ${FILE_NAME} NO contiene '${DIRECTIVE}'")
        math(EXPR FAIL_COUNT "${FAIL_COUNT} + 1")
    endif()
endmacro()

# ==============================================================================
# PASO 1: Verificar existencia de archivos CMakeLists.txt
# ==============================================================================
message(STATUS "")
message(STATUS "=== Verificando existencia de archivos CMakeLists.txt ===")
message(STATUS "")

check_file_exists("${BASE_DIR}/CMakeLists.txt" "client/CMakeLists.txt (raíz)")
check_file_exists("${BASE_DIR}/src/CMakeLists.txt" "client/src/CMakeLists.txt")
check_file_exists("${BASE_DIR}/src/game/CMakeLists.txt" "client/src/game/CMakeLists.txt")
check_file_exists("${BASE_DIR}/src/net/CMakeLists.txt" "client/src/net/CMakeLists.txt")
check_file_exists("${BASE_DIR}/src/ui/CMakeLists.txt" "client/src/ui/CMakeLists.txt")
check_file_exists("${BASE_DIR}/src/utils/CMakeLists.txt" "client/src/utils/CMakeLists.txt")

# ==============================================================================
# PASO 2: Verificar directivas en CMakeLists_Raíz (client/CMakeLists.txt)
# ==============================================================================
message(STATUS "")
message(STATUS "=== Verificando directivas en client/CMakeLists.txt ===")
message(STATUS "")

set(ROOT_CMAKE "${BASE_DIR}/CMakeLists.txt")
if(EXISTS "${ROOT_CMAKE}")
    file(READ "${ROOT_CMAKE}" ROOT_CONTENT)
    check_directive("${ROOT_CONTENT}" "cmake_minimum_required" "client/CMakeLists.txt")
    check_directive("${ROOT_CONTENT}" "project(ChronoClash" "client/CMakeLists.txt")
    check_directive("${ROOT_CONTENT}" "CMAKE_CXX_STANDARD" "client/CMakeLists.txt")
    check_directive("${ROOT_CONTENT}" "FATAL_ERROR" "client/CMakeLists.txt")
    check_directive("${ROOT_CONTENT}" "EMSCRIPTEN" "client/CMakeLists.txt")
    check_directive("${ROOT_CONTENT}" "add_subdirectory(lib/raylib)" "client/CMakeLists.txt")
    check_directive("${ROOT_CONTENT}" "add_subdirectory(src)" "client/CMakeLists.txt")
else()
    message(STATUS "[FAIL] No se puede leer client/CMakeLists.txt - archivo no existe")
    math(EXPR FAIL_COUNT "${FAIL_COUNT} + 7")
endif()

# ==============================================================================
# PASO 3: Verificar directivas en client/src/CMakeLists.txt
# ==============================================================================
message(STATUS "")
message(STATUS "=== Verificando directivas en client/src/CMakeLists.txt ===")
message(STATUS "")

set(SRC_CMAKE "${BASE_DIR}/src/CMakeLists.txt")
if(EXISTS "${SRC_CMAKE}")
    file(READ "${SRC_CMAKE}" SRC_CONTENT)
    check_directive("${SRC_CONTENT}" "add_executable(ChronoClash" "client/src/CMakeLists.txt")
    check_directive("${SRC_CONTENT}" "target_link_libraries(ChronoClash" "client/src/CMakeLists.txt")
    check_directive("${SRC_CONTENT}" "if(EMSCRIPTEN)" "client/src/CMakeLists.txt")
    check_directive("${SRC_CONTENT}" "sUSE_GLFW=3" "client/src/CMakeLists.txt")
    check_directive("${SRC_CONTENT}" "sASYNCIFY" "client/src/CMakeLists.txt")
    check_directive("${SRC_CONTENT}" "sALLOW_MEMORY_GROWTH" "client/src/CMakeLists.txt")
    check_directive("${SRC_CONTENT}" "-Wall" "client/src/CMakeLists.txt")
    check_directive("${SRC_CONTENT}" "-Wextra" "client/src/CMakeLists.txt")
else()
    message(STATUS "[FAIL] No se puede leer client/src/CMakeLists.txt - archivo no existe")
    math(EXPR FAIL_COUNT "${FAIL_COUNT} + 8")
endif()

# ==============================================================================
# PASO 4: Verificar directivas en client/src/game/CMakeLists.txt
# ==============================================================================
message(STATUS "")
message(STATUS "=== Verificando directivas en client/src/game/CMakeLists.txt ===")
message(STATUS "")

set(GAME_CMAKE "${BASE_DIR}/src/game/CMakeLists.txt")
if(EXISTS "${GAME_CMAKE}")
    file(READ "${GAME_CMAKE}" GAME_CONTENT)
    check_directive("${GAME_CONTENT}" "add_library(game STATIC" "client/src/game/CMakeLists.txt")
    check_directive("${GAME_CONTENT}" "target_link_libraries(game" "client/src/game/CMakeLists.txt")
    check_directive("${GAME_CONTENT}" "-Wall" "client/src/game/CMakeLists.txt")
    check_directive("${GAME_CONTENT}" "-Wextra" "client/src/game/CMakeLists.txt")
else()
    message(STATUS "[FAIL] No se puede leer client/src/game/CMakeLists.txt - archivo no existe")
    math(EXPR FAIL_COUNT "${FAIL_COUNT} + 4")
endif()

# ==============================================================================
# PASO 5: Verificar directivas en client/src/net/CMakeLists.txt
# ==============================================================================
message(STATUS "")
message(STATUS "=== Verificando directivas en client/src/net/CMakeLists.txt ===")
message(STATUS "")

set(NET_CMAKE "${BASE_DIR}/src/net/CMakeLists.txt")
if(EXISTS "${NET_CMAKE}")
    file(READ "${NET_CMAKE}" NET_CONTENT)
    check_directive("${NET_CONTENT}" "add_library(net STATIC" "client/src/net/CMakeLists.txt")
    check_directive("${NET_CONTENT}" "target_link_libraries(net" "client/src/net/CMakeLists.txt")
    check_directive("${NET_CONTENT}" "-Wall" "client/src/net/CMakeLists.txt")
    check_directive("${NET_CONTENT}" "-Wextra" "client/src/net/CMakeLists.txt")
else()
    message(STATUS "[FAIL] No se puede leer client/src/net/CMakeLists.txt - archivo no existe")
    math(EXPR FAIL_COUNT "${FAIL_COUNT} + 4")
endif()

# ==============================================================================
# PASO 6: Verificar directivas en client/src/ui/CMakeLists.txt
# ==============================================================================
message(STATUS "")
message(STATUS "=== Verificando directivas en client/src/ui/CMakeLists.txt ===")
message(STATUS "")

set(UI_CMAKE "${BASE_DIR}/src/ui/CMakeLists.txt")
if(EXISTS "${UI_CMAKE}")
    file(READ "${UI_CMAKE}" UI_CONTENT)
    check_directive("${UI_CONTENT}" "add_library(ui STATIC" "client/src/ui/CMakeLists.txt")
    check_directive("${UI_CONTENT}" "target_link_libraries(ui" "client/src/ui/CMakeLists.txt")
    check_directive("${UI_CONTENT}" "-Wall" "client/src/ui/CMakeLists.txt")
    check_directive("${UI_CONTENT}" "-Wextra" "client/src/ui/CMakeLists.txt")
else()
    message(STATUS "[FAIL] No se puede leer client/src/ui/CMakeLists.txt - archivo no existe")
    math(EXPR FAIL_COUNT "${FAIL_COUNT} + 4")
endif()

# ==============================================================================
# PASO 7: Verificar directivas en client/src/utils/CMakeLists.txt
# ==============================================================================
message(STATUS "")
message(STATUS "=== Verificando directivas en client/src/utils/CMakeLists.txt ===")
message(STATUS "")

set(UTILS_CMAKE "${BASE_DIR}/src/utils/CMakeLists.txt")
if(EXISTS "${UTILS_CMAKE}")
    file(READ "${UTILS_CMAKE}" UTILS_CONTENT)
    check_directive("${UTILS_CONTENT}" "add_library(utils INTERFACE" "client/src/utils/CMakeLists.txt")
    check_directive("${UTILS_CONTENT}" "target_include_directories(utils" "client/src/utils/CMakeLists.txt")
else()
    message(STATUS "[FAIL] No se puede leer client/src/utils/CMakeLists.txt - archivo no existe")
    math(EXPR FAIL_COUNT "${FAIL_COUNT} + 2")
endif()

# ==============================================================================
# RESUMEN
# ==============================================================================
message(STATUS "")
message(STATUS "===============================================")
message(STATUS "  RESUMEN DE VERIFICACIÓN")
message(STATUS "===============================================")
message(STATUS "  Checks pasados: ${PASS_COUNT}")
message(STATUS "  Checks fallidos: ${FAIL_COUNT}")
message(STATUS "===============================================")
message(STATUS "")

if(FAIL_COUNT GREATER 0)
    message(FATAL_ERROR "La verificación falló con ${FAIL_COUNT} error(es). Revisa los mensajes [FAIL] anteriores.")
endif()

message(STATUS "Todos los checks pasaron exitosamente.")
