# Tests de estructura CMake

## Verificación de estructura

El script `verify_structure.cmake` valida que todos los archivos CMakeLists.txt del proyecto existen y contienen las directivas esperadas.

### Cómo ejecutar

Desde la raíz del repositorio (`hackathon-kiro-AI/`):

```bash
cmake -P client/tests/verify_structure.cmake
```

### Qué verifica

1. **Existencia de archivos**: Los 6 archivos CMakeLists.txt (raíz, src, game, net, ui, utils)
2. **Directivas del CMakeLists raíz**: cmake_minimum_required, project, C++17, verificación de Raylib, soporte Emscripten
3. **Directivas de src/**: Target ejecutable, enlace de bibliotecas, flags de Emscripten (GLFW, ASYNCIFY, MEMORY_GROWTH), warnings
4. **Directivas de módulos (game, net, ui)**: Biblioteca estática, dependencias, warnings (-Wall -Wextra)
5. **Directivas de utils**: Biblioteca INTERFACE, rutas de inclusión

### Salida esperada

- `[PASS]` para cada check que pasa correctamente
- `[FAIL]` para cada check que falla
- Resumen final con conteo de éxitos y fallos
- Código de salida 0 si todo pasa, distinto de 0 si hay fallos

---

## Tests de condiciones de error

El script `test_error_conditions.cmake` verifica que el sistema de build falla correctamente con mensajes informativos cuando se dan condiciones de error conocidas.

### Cómo ejecutar

Desde la raíz del repositorio (`hackathon-kiro-AI/`):

```bash
cmake -P client/tests/test_error_conditions.cmake
```

### Qué verifica

1. **Submódulo Raylib no inicializado** (Requisito 3.4, 5.4):
   - Crea un directorio temporal con solo el CMakeLists.txt raíz (sin `lib/raylib/`)
   - Ejecuta cmake contra ese directorio
   - Verifica que cmake falla con código != 0
   - Verifica que el mensaje de error contiene instrucciones sobre el submódulo (menciona "submodule", "Raylib" o "git submodule")
   - Verifica que el error es de tipo FATAL_ERROR

2. **Toolchain inválido** (Requisito 5.3):
   - Ejecuta cmake con `-DCMAKE_TOOLCHAIN_FILE` apuntando a un archivo inexistente
   - Verifica que cmake falla con código != 0
   - Verifica que el mensaje de error referencia el archivo de toolchain

### Notas

- Este script NO requiere Emscripten SDK instalado
- Los directorios temporales se crean y limpian automáticamente
- Cada test es independiente y se ejecuta en su propio directorio aislado

### Salida esperada

- `[PASS]` para cada condición verificada correctamente
- `[FAIL]` para cada verificación que no se cumple (incluye stdout/stderr para diagnóstico)
- Resumen final con conteo de tests pasados y fallidos
- Código de salida 0 si todo pasa, distinto de 0 si hay fallos
