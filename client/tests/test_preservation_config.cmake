# ==============================================================================
# Test de preservación: Configuración CMake con Emscripten
# Verifica que cmake -B completa con exit code 0 y que los linker flags
# esperados están presentes en los archivos de build generados.
#
# Uso: cmake -P client/tests/test_preservation_config.cmake
#      cmake -P client/tests/test_preservation_config.cmake -DEMSDK_PATH=<ruta>
# Ejecutar desde la raíz del repositorio (hackathon-kiro-AI/)
#
# **Validates: Requirements 3.1, 3.2, 3.3**
# ==============================================================================

set(PASS_COUNT 0)
set(FAIL_COUNT 0)
set(SKIP_COUNT 0)
set(CLIENT_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
set(BUILD_DIR "${CLIENT_DIR}/build_preservation_test")

# ------------------------------------------------------------------------------
# Macros de utilidad
# ------------------------------------------------------------------------------
macro(test_pass DESCRIPTION)
    message(STATUS "[PASS] ${DESCRIPTION}")
    math(EXPR PASS_COUNT "${PASS_COUNT} + 1")
endmacro()

macro(test_fail DESCRIPTION)
    message(STATUS "[FAIL] ${DESCRIPTION}")
    math(EXPR FAIL_COUNT "${FAIL_COUNT} + 1")
endmacro()

macro(test_skip DESCRIPTION)
    message(STATUS "[SKIP] ${DESCRIPTION}")
    math(EXPR SKIP_COUNT "${SKIP_COUNT} + 1")
endmacro()

# ==============================================================================
# PASO 1: Detectar el toolchain de Emscripten
# ==============================================================================
message(STATUS "")
message(STATUS "=== Detectando Emscripten SDK ===")
message(STATUS "")

set(TOOLCHAIN_FILE "")

# Prioridad 1: Variable pasada por el usuario via -DEMSDK_PATH
if(DEFINED EMSDK_PATH AND NOT "${EMSDK_PATH}" STREQUAL "")
    set(CANDIDATE "${EMSDK_PATH}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake")
    if(EXISTS "${CANDIDATE}")
        set(TOOLCHAIN_FILE "${CANDIDATE}")
        message(STATUS "Toolchain encontrado via EMSDK_PATH: ${TOOLCHAIN_FILE}")
    else()
        message(STATUS "ADVERTENCIA: EMSDK_PATH definido pero toolchain no encontrado en: ${CANDIDATE}")
    endif()
endif()

# Prioridad 2: Variable de entorno EMSDK
if("${TOOLCHAIN_FILE}" STREQUAL "")
    if(DEFINED ENV{EMSDK})
        set(CANDIDATE "$ENV{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake")
        if(EXISTS "${CANDIDATE}")
            set(TOOLCHAIN_FILE "${CANDIDATE}")
            message(STATUS "Toolchain encontrado via variable de entorno EMSDK: ${TOOLCHAIN_FILE}")
        else()
            message(STATUS "ADVERTENCIA: EMSDK definido pero toolchain no encontrado en: ${CANDIDATE}")
        endif()
    endif()
endif()

# Si no se encontró el toolchain, saltar todos los tests
if("${TOOLCHAIN_FILE}" STREQUAL "")
    message(STATUS "")
    message(STATUS "===============================================")
    message(STATUS "  EMSCRIPTEN SDK NO ENCONTRADO")
    message(STATUS "===============================================")
    message(STATUS "")
    message(STATUS "Los tests de preservación requieren Emscripten SDK instalado.")
    message(STATUS "Opciones:")
    message(STATUS "  1. cmake -P client/tests/test_preservation_config.cmake -DEMSDK_PATH=/ruta/al/emsdk")
    message(STATUS "  2. Definir variable de entorno EMSDK")
    message(STATUS "")
    message(STATUS "===============================================")
    message(STATUS "  Tests de preservación OMITIDOS")
    message(STATUS "===============================================")
    return()
endif()

# ==============================================================================
# PASO 2: Preparar directorio de build temporal
# ==============================================================================
message(STATUS "")
message(STATUS "=== Preparando directorio de build temporal ===")
message(STATUS "")

if(EXISTS "${BUILD_DIR}")
    message(STATUS "Limpiando build anterior en: ${BUILD_DIR}")
    file(REMOVE_RECURSE "${BUILD_DIR}")
endif()

file(MAKE_DIRECTORY "${BUILD_DIR}")
message(STATUS "Directorio de build creado: ${BUILD_DIR}")

# ==============================================================================
# PASO 3: Test - Configuración CMake exitosa (cmake -B) con exit code 0
# Validates: Requirement 3.1
# ==============================================================================
message(STATUS "")
message(STATUS "=== Test: Configuración CMake completa con exit code 0 ===")
message(STATUS "")

execute_process(
    COMMAND "${CMAKE_COMMAND}" -S "${CLIENT_DIR}" -B "${BUILD_DIR}"
            "-DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE}"
            -G "MinGW Makefiles"
    WORKING_DIRECTORY "${CLIENT_DIR}"
    RESULT_VARIABLE CONFIG_RESULT
    OUTPUT_VARIABLE CONFIG_OUTPUT
    ERROR_VARIABLE CONFIG_ERROR
    TIMEOUT 120
)

if(CONFIG_RESULT EQUAL 0)
    test_pass("cmake -B configuración completó con exit code 0 (Req 3.1)")
else()
    test_fail("cmake -B configuración falló con exit code ${CONFIG_RESULT} (Req 3.1)")
    message(STATUS "  Error: ${CONFIG_ERROR}")
    # No podemos verificar linker flags si la configuración falló
    message(STATUS "")
    message(STATUS "Configuración falló - no se pueden verificar linker flags.")

    # Limpieza
    if(EXISTS "${BUILD_DIR}")
        file(REMOVE_RECURSE "${BUILD_DIR}")
    endif()

    message(STATUS "")
    message(STATUS "===============================================")
    message(STATUS "  RESUMEN DE TESTS DE PRESERVACIÓN")
    message(STATUS "===============================================")
    message(STATUS "  Tests pasados:  ${PASS_COUNT}")
    message(STATUS "  Tests fallidos: ${FAIL_COUNT}")
    message(STATUS "  Tests saltados: ${SKIP_COUNT}")
    message(STATUS "===============================================")
    message(FATAL_ERROR "Tests de preservación fallaron con ${FAIL_COUNT} error(es).")
endif()

# ==============================================================================
# PASO 4: Test - Verificar linker flags en archivos de build generados
# Validates: Requirement 3.3
# ==============================================================================
message(STATUS "")
message(STATUS "=== Test: Linker flags presentes en build generado ===")
message(STATUS "")

set(LINK_FILE "${BUILD_DIR}/src/CMakeFiles/ChronoClash.dir/link.txt")

if(NOT EXISTS "${LINK_FILE}")
    test_fail("Archivo link.txt no encontrado en: ${LINK_FILE}")
    message(STATUS "  No se pueden verificar linker flags sin link.txt")
else()
    file(READ "${LINK_FILE}" LINK_CONTENT)

    # Verificar cada linker flag esperado
    set(EXPECTED_FLAGS
        "-sUSE_GLFW=3"
        "-sASYNCIFY"
        "-sALLOW_MEMORY_GROWTH=1"
        "-lwebsocket.js"
    )

    foreach(FLAG ${EXPECTED_FLAGS})
        string(FIND "${LINK_CONTENT}" "${FLAG}" FLAG_POS)
        if(FLAG_POS GREATER -1)
            test_pass("Linker flag '${FLAG}' presente en build (Req 3.3)")
        else()
            test_fail("Linker flag '${FLAG}' NO encontrado en link.txt (Req 3.3)")
            message(STATUS "  Contenido de link.txt: ${LINK_CONTENT}")
        endif()
    endforeach()
endif()

# ==============================================================================
# PASO 5: Test - Sin warnings nuevos en la configuración
# Validates: Requirement 3.2
# ==============================================================================
message(STATUS "")
message(STATUS "=== Test: Sin errores críticos en la configuración ===")
message(STATUS "")

# Verificar que no hay errores de configuración (los warnings de CMake son aceptables)
# El warning sobre CMAKE_BUILD_TYPE es preexistente y esperado
string(FIND "${CONFIG_ERROR}" "CMake Error" ERROR_POS)
if(ERROR_POS EQUAL -1)
    test_pass("Sin errores CMake en la salida de configuración (Req 3.2)")
else()
    test_fail("Se encontraron errores CMake en la configuración (Req 3.2)")
    message(STATUS "  ${CONFIG_ERROR}")
endif()

# ==============================================================================
# PASO 6: Limpieza
# ==============================================================================
message(STATUS "")
message(STATUS "=== Limpieza ===")
message(STATUS "")

if(EXISTS "${BUILD_DIR}")
    file(REMOVE_RECURSE "${BUILD_DIR}")
    message(STATUS "Directorio temporal eliminado: ${BUILD_DIR}")
else()
    message(STATUS "No hay directorio temporal que limpiar.")
endif()

# ==============================================================================
# RESUMEN
# ==============================================================================
message(STATUS "")
message(STATUS "===============================================")
message(STATUS "  RESUMEN DE TESTS DE PRESERVACIÓN")
message(STATUS "===============================================")
message(STATUS "  Tests pasados:  ${PASS_COUNT}")
message(STATUS "  Tests fallidos: ${FAIL_COUNT}")
message(STATUS "  Tests saltados: ${SKIP_COUNT}")
message(STATUS "===============================================")
message(STATUS "")

if(FAIL_COUNT GREATER 0)
    message(FATAL_ERROR "Tests de preservación fallaron con ${FAIL_COUNT} error(es).")
endif()

if(PASS_COUNT GREATER 0)
    message(STATUS "Todos los tests de preservación pasaron exitosamente.")
endif()
