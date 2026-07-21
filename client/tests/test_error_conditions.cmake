# ==============================================================================
# Script de verificación de condiciones de error CMake para ChronoClash
# Uso: cmake -P client/tests/test_error_conditions.cmake
# Ejecutar desde la raíz del repositorio (hackathon-kiro-AI/)
# ==============================================================================

set(PASS_COUNT 0)
set(FAIL_COUNT 0)
set(CLIENT_DIR "${CMAKE_CURRENT_LIST_DIR}/..")

# Buscar el ejecutable de CMake
find_program(CMAKE_COMMAND_PATH cmake)
if(NOT CMAKE_COMMAND_PATH)
    message(FATAL_ERROR "No se encontró el ejecutable de cmake en el PATH")
endif()

# Directorio temporal base para los tests
set(TEMP_BASE "${CMAKE_CURRENT_LIST_DIR}/_test_temp")

# Limpiar directorio temporal si existe de una ejecución anterior
if(EXISTS "${TEMP_BASE}")
    file(REMOVE_RECURSE "${TEMP_BASE}")
endif()
file(MAKE_DIRECTORY "${TEMP_BASE}")

# ==============================================================================
# TEST 1: Submódulo Raylib no inicializado produce FATAL_ERROR
# Requisito 3.4: Si client/lib/raylib/ no contiene CMakeLists.txt, cmake SHALL
# emitir FATAL_ERROR con instrucciones para ejecutar git submodule update --init
# ==============================================================================
message(STATUS "")
message(STATUS "=== TEST 1: Submódulo Raylib no inicializado ===")
message(STATUS "")

# Crear directorio temporal con solo el CMakeLists.txt raíz (sin lib/raylib/)
set(TEST1_SRC "${TEMP_BASE}/test1_src")
set(TEST1_BUILD "${TEMP_BASE}/test1_build")
file(MAKE_DIRECTORY "${TEST1_SRC}")
file(MAKE_DIRECTORY "${TEST1_BUILD}")

# Copiar solo el CMakeLists.txt raíz al directorio temporal
file(COPY "${CLIENT_DIR}/CMakeLists.txt" DESTINATION "${TEST1_SRC}")

# NO crear lib/raylib/CMakeLists.txt - esto simula el submódulo no inicializado

# Ejecutar cmake y capturar resultado
execute_process(
    COMMAND "${CMAKE_COMMAND_PATH}" -B "${TEST1_BUILD}" -S "${TEST1_SRC}"
    RESULT_VARIABLE TEST1_RESULT
    OUTPUT_VARIABLE TEST1_OUTPUT
    ERROR_VARIABLE TEST1_ERROR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_STRIP_TRAILING_WHITESPACE
)

# Verificar que cmake falló (código != 0)
if(NOT TEST1_RESULT EQUAL 0)
    message(STATUS "[PASS] cmake falló con código ${TEST1_RESULT} (esperado != 0)")
    math(EXPR PASS_COUNT "${PASS_COUNT} + 1")
else()
    message(STATUS "[FAIL] cmake retornó código 0 pero se esperaba un error")
    math(EXPR FAIL_COUNT "${FAIL_COUNT} + 1")
endif()

# Verificar que el mensaje de error contiene información instructiva
# Combinar stdout y stderr para buscar el mensaje
set(TEST1_COMBINED "${TEST1_OUTPUT} ${TEST1_ERROR}")

# Buscar indicadores del mensaje instructivo sobre el submódulo
set(TEST1_HAS_MESSAGE FALSE)

string(FIND "${TEST1_COMBINED}" "submodule" _pos1)
string(FIND "${TEST1_COMBINED}" "Raylib" _pos2)
string(FIND "${TEST1_COMBINED}" "git submodule" _pos3)

if(_pos1 GREATER -1 OR _pos2 GREATER -1 OR _pos3 GREATER -1)
    set(TEST1_HAS_MESSAGE TRUE)
endif()

if(TEST1_HAS_MESSAGE)
    message(STATUS "[PASS] El mensaje de error contiene instrucciones sobre el submódulo")
    math(EXPR PASS_COUNT "${PASS_COUNT} + 1")
else()
    message(STATUS "[FAIL] El mensaje de error no contiene instrucciones sobre el submódulo")
    message(STATUS "       stdout: ${TEST1_OUTPUT}")
    message(STATUS "       stderr: ${TEST1_ERROR}")
    math(EXPR FAIL_COUNT "${FAIL_COUNT} + 1")
endif()

# Verificar que se menciona FATAL_ERROR o es un error fatal
string(FIND "${TEST1_COMBINED}" "FATAL_ERROR" _pos_fatal)
string(FIND "${TEST1_COMBINED}" "CMake Error" _pos_cmake_err)

if(_pos_fatal GREATER -1 OR _pos_cmake_err GREATER -1)
    message(STATUS "[PASS] El error es de tipo FATAL_ERROR/CMake Error")
    math(EXPR PASS_COUNT "${PASS_COUNT} + 1")
else()
    message(STATUS "[FAIL] No se detectó un error de tipo FATAL_ERROR")
    math(EXPR FAIL_COUNT "${FAIL_COUNT} + 1")
endif()

# ==============================================================================
# TEST 2: Toolchain inválido produce error con código != 0
# Requisito 5.3: Si CMAKE_TOOLCHAIN_FILE no existe, cmake SHALL terminar con
# código de salida != 0 y emitir un error nombrando el toolchain faltante
# ==============================================================================
message(STATUS "")
message(STATUS "=== TEST 2: Toolchain inválido ===")
message(STATUS "")

set(TEST2_BUILD "${TEMP_BASE}/test2_build")
file(MAKE_DIRECTORY "${TEST2_BUILD}")

# Usar una ruta que definitivamente no existe como toolchain file
set(INVALID_TOOLCHAIN "/nonexistent/path/to/invalid_toolchain.cmake")

# Ejecutar cmake con un toolchain inexistente contra el directorio real del cliente
execute_process(
    COMMAND "${CMAKE_COMMAND_PATH}" -B "${TEST2_BUILD}" -S "${CLIENT_DIR}"
        "-DCMAKE_TOOLCHAIN_FILE=${INVALID_TOOLCHAIN}"
    RESULT_VARIABLE TEST2_RESULT
    OUTPUT_VARIABLE TEST2_OUTPUT
    ERROR_VARIABLE TEST2_ERROR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_STRIP_TRAILING_WHITESPACE
)

# Verificar que cmake falló (código != 0)
if(NOT TEST2_RESULT EQUAL 0)
    message(STATUS "[PASS] cmake falló con código ${TEST2_RESULT} cuando se usó toolchain inválido")
    math(EXPR PASS_COUNT "${PASS_COUNT} + 1")
else()
    message(STATUS "[FAIL] cmake retornó código 0 con toolchain inválido (se esperaba error)")
    math(EXPR FAIL_COUNT "${FAIL_COUNT} + 1")
endif()

# Verificar que el error menciona el toolchain o la ruta
set(TEST2_COMBINED "${TEST2_OUTPUT} ${TEST2_ERROR}")
string(FIND "${TEST2_COMBINED}" "toolchain" _pos_tc1)
string(FIND "${TEST2_COMBINED}" "TOOLCHAIN" _pos_tc2)
string(FIND "${TEST2_COMBINED}" "${INVALID_TOOLCHAIN}" _pos_tc3)
string(FIND "${TEST2_COMBINED}" "nonexistent" _pos_tc4)

if(_pos_tc1 GREATER -1 OR _pos_tc2 GREATER -1 OR _pos_tc3 GREATER -1 OR _pos_tc4 GREATER -1)
    message(STATUS "[PASS] El mensaje de error referencia el toolchain file")
    math(EXPR PASS_COUNT "${PASS_COUNT} + 1")
else()
    message(STATUS "[FAIL] El mensaje de error no referencia el toolchain file")
    message(STATUS "       stdout: ${TEST2_OUTPUT}")
    message(STATUS "       stderr: ${TEST2_ERROR}")
    math(EXPR FAIL_COUNT "${FAIL_COUNT} + 1")
endif()

# ==============================================================================
# LIMPIEZA
# ==============================================================================
message(STATUS "")
message(STATUS "=== Limpiando directorios temporales ===")
message(STATUS "")

file(REMOVE_RECURSE "${TEMP_BASE}")
message(STATUS "Directorio temporal eliminado: ${TEMP_BASE}")

# ==============================================================================
# RESUMEN
# ==============================================================================
message(STATUS "")
message(STATUS "===============================================")
message(STATUS "  RESUMEN DE TESTS DE CONDICIONES DE ERROR")
message(STATUS "===============================================")
message(STATUS "  Tests pasados: ${PASS_COUNT}")
message(STATUS "  Tests fallidos: ${FAIL_COUNT}")
message(STATUS "===============================================")
message(STATUS "")

if(FAIL_COUNT GREATER 0)
    message(FATAL_ERROR "Los tests de condiciones de error fallaron con ${FAIL_COUNT} error(es).")
endif()

message(STATUS "Todos los tests de condiciones de error pasaron exitosamente.")
