# ==============================================================================
# Test de integración de build para ChronoClash (Emscripten/WebAssembly)
# Uso: cmake -P client/tests/test_integration_build.cmake
#      cmake -P client/tests/test_integration_build.cmake -DEMSDK_PATH=<ruta_al_sdk>
# Ejecutar desde la raíz del repositorio (hackathon-kiro-AI/)
# ==============================================================================

set(PASS_COUNT 0)
set(FAIL_COUNT 0)
set(SKIP_COUNT 0)
set(CLIENT_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
set(BUILD_DIR "${CLIENT_DIR}/build_test_integration")

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

# Si no se encontró el toolchain, saltar todos los tests gracefully
if("${TOOLCHAIN_FILE}" STREQUAL "")
    message(STATUS "")
    message(STATUS "===============================================")
    message(STATUS "  EMSCRIPTEN SDK NO ENCONTRADO")
    message(STATUS "===============================================")
    message(STATUS "")
    message(STATUS "Los tests de integración requieren Emscripten SDK instalado.")
    message(STATUS "Para ejecutar estos tests, usa una de las siguientes opciones:")
    message(STATUS "")
    message(STATUS "  1. Definir la variable EMSDK_PATH:")
    message(STATUS "     cmake -P client/tests/test_integration_build.cmake -DEMSDK_PATH=/ruta/al/emsdk")
    message(STATUS "")
    message(STATUS "  2. Definir la variable de entorno EMSDK:")
    message(STATUS "     set EMSDK=/ruta/al/emsdk  (Windows)")
    message(STATUS "     export EMSDK=/ruta/al/emsdk  (Linux/macOS)")
    message(STATUS "")
    message(STATUS "  El archivo de toolchain esperado es:")
    message(STATUS "  <EMSDK>/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake")
    message(STATUS "")
    message(STATUS "===============================================")
    message(STATUS "  Tests de integración OMITIDOS (Emscripten no disponible)")
    message(STATUS "===============================================")
    # Salir sin error - esto no es un fallo, es un skip
    return()
endif()

# ==============================================================================
# PASO 2: Preparar directorio de build temporal
# ==============================================================================
message(STATUS "")
message(STATUS "=== Preparando directorio de build temporal ===")
message(STATUS "")

# Limpiar build anterior si existe
if(EXISTS "${BUILD_DIR}")
    message(STATUS "Limpiando build anterior en: ${BUILD_DIR}")
    file(REMOVE_RECURSE "${BUILD_DIR}")
endif()

file(MAKE_DIRECTORY "${BUILD_DIR}")
message(STATUS "Directorio de build creado: ${BUILD_DIR}")

# ==============================================================================
# PASO 3: Test - Configuración CMake exitosa (cmake -B build)
# ==============================================================================
message(STATUS "")
message(STATUS "=== Test: Configuración CMake (cmake -B) ===")
message(STATUS "")

execute_process(
    COMMAND "${CMAKE_COMMAND}" -B "${BUILD_DIR}" "-DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE}"
    WORKING_DIRECTORY "${CLIENT_DIR}"
    RESULT_VARIABLE CONFIG_RESULT
    OUTPUT_VARIABLE CONFIG_OUTPUT
    ERROR_VARIABLE CONFIG_ERROR
    TIMEOUT 120
)

if(CONFIG_RESULT EQUAL 0)
    test_pass("cmake -B build -DCMAKE_TOOLCHAIN_FILE=... completó con código 0")
else()
    test_fail("cmake -B build falló con código ${CONFIG_RESULT}")
    message(STATUS "  Salida de error:")
    message(STATUS "  ${CONFIG_ERROR}")
    # Si la configuración falla, no podemos continuar con el build
    message(STATUS "")
    message(STATUS "La configuración falló. Saltando tests de compilación y artefactos.")
    set(BUILD_FAILED TRUE)
endif()

# ==============================================================================
# PASO 4: Test - Compilación exitosa (cmake --build build)
# ==============================================================================
if(NOT BUILD_FAILED)
    message(STATUS "")
    message(STATUS "=== Test: Compilación (cmake --build) ===")
    message(STATUS "")

    execute_process(
        COMMAND "${CMAKE_COMMAND}" --build "${BUILD_DIR}"
        WORKING_DIRECTORY "${CLIENT_DIR}"
        RESULT_VARIABLE BUILD_RESULT
        OUTPUT_VARIABLE BUILD_OUTPUT
        ERROR_VARIABLE BUILD_ERROR
        TIMEOUT 300
    )

    if(BUILD_RESULT EQUAL 0)
        test_pass("cmake --build build completó con código 0")
    else()
        test_fail("cmake --build build falló con código ${BUILD_RESULT}")
        message(STATUS "  Salida de error:")
        message(STATUS "  ${BUILD_ERROR}")
        set(BUILD_FAILED TRUE)
    endif()
endif()

# ==============================================================================
# PASO 5: Test - Verificar artefactos generados (tamaño > 0)
# ==============================================================================
message(STATUS "")
message(STATUS "=== Test: Verificación de artefactos ===")
message(STATUS "")

set(ARTIFACTS index.html index.js index.wasm index.data)

if(BUILD_FAILED)
    foreach(ARTIFACT ${ARTIFACTS})
        test_skip("Artefacto ${ARTIFACT} - build no completó exitosamente")
    endforeach()
else()
    foreach(ARTIFACT ${ARTIFACTS})
        set(ARTIFACT_PATH "${BUILD_DIR}/${ARTIFACT}")
        if(EXISTS "${ARTIFACT_PATH}")
            file(SIZE "${ARTIFACT_PATH}" ARTIFACT_SIZE)
            if(ARTIFACT_SIZE GREATER 0)
                test_pass("${ARTIFACT} existe con tamaño ${ARTIFACT_SIZE} bytes (> 0)")
            else()
                test_fail("${ARTIFACT} existe pero tiene tamaño 0 bytes")
            endif()
        else()
            test_fail("${ARTIFACT} NO existe en ${BUILD_DIR}/")
        endif()
    endforeach()
endif()

# ==============================================================================
# PASO 6: Limpieza del directorio de build temporal
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
message(STATUS "  RESUMEN DE TESTS DE INTEGRACIÓN")
message(STATUS "===============================================")
message(STATUS "  Tests pasados:  ${PASS_COUNT}")
message(STATUS "  Tests fallidos: ${FAIL_COUNT}")
message(STATUS "  Tests saltados: ${SKIP_COUNT}")
message(STATUS "===============================================")
message(STATUS "")

if(FAIL_COUNT GREATER 0)
    message(FATAL_ERROR "Los tests de integración fallaron con ${FAIL_COUNT} error(es).")
endif()

if(PASS_COUNT GREATER 0)
    message(STATUS "Todos los tests de integración pasaron exitosamente.")
elseif(SKIP_COUNT GREATER 0)
    message(STATUS "Tests de integración saltados (build no disponible).")
endif()
