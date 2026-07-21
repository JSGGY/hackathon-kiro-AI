# Documento de Requisitos

## Introducción

Este documento define los requisitos para migrar el sistema de build del cliente ChronoClash de un Makefile a CMake. El objetivo es establecer una configuración CMake modular que permita compilar el proyecto C++/Raylib a WebAssembly mediante Emscripten, manteniendo una estructura organizada que facilite la incorporación de nuevos componentes sin intervención manual.

## Glosario

- **Sistema_Build**: El sistema de configuración y compilación basado en CMake que gestiona targets, dependencias y opciones de compilación del proyecto ChronoClash.
- **Target_Principal**: El ejecutable final del cliente ChronoClash que se genera como resultado de la compilación (index.html, index.js, index.wasm, index.data).
- **Módulo**: Una unidad lógica del proyecto (game, net, ui, utils) representada como una biblioteca estática con su propio CMakeLists.txt.
- **Toolchain_Emscripten**: El archivo de toolchain de CMake proporcionado por Emscripten (Emscripten.cmake) que configura el compilador y enlazador para generar WebAssembly.
- **Preload_Assets**: El mecanismo de Emscripten para empaquetar archivos de assets en el bundle final mediante el flag --preload-file.
- **CMakeLists_Raíz**: El archivo CMakeLists.txt principal ubicado en el directorio client/ que orquesta la configuración global del proyecto.
- **Raylib**: Biblioteca de gráficos 3D utilizada como dependencia principal del proyecto, integrada como submódulo Git en client/lib/raylib/.

## Requisitos

### Requisito 1: Estructura de archivos CMake

**Historia de Usuario:** Como desarrollador, quiero tener una estructura de archivos CMakeLists.txt organizada jerárquicamente, para que cada módulo del proyecto sea independiente y fácil de mantener.

#### Criterios de Aceptación

1. THE Sistema_Build SHALL proporcionar un CMakeLists_Raíz en el directorio client/ que defina el proyecto con cmake_minimum_required versión 3.15 o superior, el estándar C++17 como requisito de compilación, y las opciones globales de compilación (warnings y nivel de optimización según tipo de build).
2. THE Sistema_Build SHALL proporcionar un CMakeLists.txt en cada subdirectorio de módulo (src/game/, src/net/, src/ui/, src/utils/) que defina el Módulo como una biblioteca estática, liste explícitamente los archivos fuente (.cpp) y headers (.hpp) del módulo, y declare sus dependencias de enlace hacia otros Módulos o bibliotecas externas mediante target_link_libraries.
3. THE Sistema_Build SHALL proporcionar un CMakeLists.txt en src/ que incluya cada subdirectorio de módulo mediante add_subdirectory y defina el Target_Principal como un ejecutable enlazado a todos los Módulos.
4. WHEN un desarrollador agregue un archivo fuente a un Módulo existente, THEN THE Sistema_Build SHALL requerir únicamente añadir el archivo a la lista de fuentes del CMakeLists.txt del Módulo correspondiente para que sea incluido en la compilación.

### Requisito 2: Configuración del target principal

**Historia de Usuario:** Como desarrollador, quiero que el target principal del ejecutable esté correctamente configurado, para que el proyecto compile y enlace todos los módulos y dependencias necesarias.

#### Criterios de Aceptación

1. THE Sistema_Build SHALL definir el Target_Principal como un ejecutable que compile los archivos fuente de todos los Módulos (game, net, ui, utils) junto con el punto de entrada main.cpp, y enlace la biblioteca Raylib.
2. THE Sistema_Build SHALL configurar el estándar C++17 como requisito mínimo para el Target_Principal.
3. THE Sistema_Build SHALL configurar las rutas de inclusión de headers para que cada Módulo pueda resolver includes de sus propios headers, de los headers de otros Módulos del proyecto y de los headers de Raylib (client/lib/raylib/src).
4. THE Sistema_Build SHALL configurar los flags de enlace de Emscripten (USE_GLFW=3, ASYNCIFY, ALLOW_MEMORY_GROWTH) y producir como salida los artefactos index.html, index.js, index.wasm e index.data en el directorio de build.
5. THE Sistema_Build SHALL configurar el flag --preload-file para empaquetar el directorio assets dentro del artefacto index.data y el flag --shell-file apuntando a src/shell.html como plantilla HTML.

### Requisito 3: Integración de Raylib como dependencia

**Historia de Usuario:** Como desarrollador, quiero que Raylib se integre automáticamente en el sistema de build, para no tener que gestionar manualmente su compilación ni sus rutas de inclusión.

#### Criterios de Aceptación

1. THE Sistema_Build SHALL integrar Raylib desde el directorio client/lib/raylib/ mediante add_subdirectory de CMake.
2. WHEN se compile con Toolchain_Emscripten (variable EMSCRIPTEN definida), THE Sistema_Build SHALL configurar la variable PLATFORM de Raylib con el valor "Web" antes de invocar add_subdirectory sobre el directorio de Raylib.
3. THE Sistema_Build SHALL propagar las rutas de inclusión de Raylib (client/lib/raylib/src/) al Target_Principal mediante target_link_libraries enlazando al target raylib, haciendo que los headers estén disponibles sin directivas include_directories adicionales.
4. IF el directorio client/lib/raylib/ no contiene el archivo CMakeLists.txt de Raylib (submódulo no inicializado), THEN THE Sistema_Build SHALL emitir un mensaje FATAL_ERROR indicando que el submódulo de Raylib no está inicializado y las instrucciones para ejecutar git submodule update --init.

### Requisito 4: Soporte de compilación con Emscripten

**Historia de Usuario:** Como desarrollador, quiero poder compilar el proyecto a WebAssembly usando Emscripten, para que el juego se ejecute en navegadores web.

#### Criterios de Aceptación

1. WHEN se invoque CMake con el Toolchain_Emscripten, THE Sistema_Build SHALL detectar la compilación Emscripten mediante la variable EMSCRIPTEN de CMake y configurar el sufijo de salida del Target_Principal como .html para generar los archivos index.html, index.js e index.wasm.
2. WHEN se compile con Toolchain_Emscripten, THE Sistema_Build SHALL configurar las flags de runtime ASYNCIFY, ALLOW_MEMORY_GROWTH=1 y USE_GLFW=3 como flags de enlace del Target_Principal.
3. WHEN se compile con Toolchain_Emscripten, THE Sistema_Build SHALL configurar el flag --preload-file para empaquetar el directorio assets/ (relativo al directorio client/) en index.data.
4. WHEN se compile con Toolchain_Emscripten, THE Sistema_Build SHALL utilizar el archivo client/src/shell.html como template HTML personalizado mediante el flag --shell-file.
5. WHEN se compile con Toolchain_Emscripten, THE Sistema_Build SHALL habilitar el soporte de WebSocket de Emscripten mediante la flag -lwebsocket.js para que las APIs emscripten_websocket_* estén disponibles en tiempo de enlace.

### Requisito 5: Flujo de configuración y compilación estándar

**Historia de Usuario:** Como desarrollador, quiero poder configurar y compilar el proyecto usando los comandos estándar de CMake, para no depender de scripts personalizados ni pasos manuales.

#### Criterios de Aceptación

1. WHEN un desarrollador ejecute la secuencia `cmake -B build -DCMAKE_TOOLCHAIN_FILE=<ruta_emsdk>/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake` seguido de `cmake --build build` desde el directorio `client/`, THE Sistema_Build SHALL completar ambos comandos con código de salida 0 y generar los artefactos de salida sin requerir intervención manual adicional entre pasos.
2. WHEN la compilación finalice con código de salida 0, THE Sistema_Build SHALL crear el directorio `client/build/` conteniendo al menos los archivos index.html, index.js, index.wasm e index.data, cada uno con tamaño mayor a 0 bytes.
3. IF la configuración de CMake falla porque el archivo de toolchain especificado en CMAKE_TOOLCHAIN_FILE no existe o no es válido, THEN THE Sistema_Build SHALL terminar con código de salida distinto de 0 y emitir un mensaje de error que nombre el archivo de toolchain no encontrado.
4. IF la configuración de CMake falla porque una dependencia requerida (Raylib, fuentes del proyecto) no se encuentra en la ruta esperada, THEN THE Sistema_Build SHALL terminar con código de salida distinto de 0 y emitir un mensaje de error que identifique la dependencia faltante por nombre.
5. THE Sistema_Build SHALL permitir la configuración sin requerir que la variable CMAKE_TOOLCHAIN_FILE apunte a una ruta absoluta fija, aceptando cualquier ruta válida donde el SDK de Emscripten esté instalado.

### Requisito 6: Modularidad y extensibilidad

**Historia de Usuario:** Como desarrollador, quiero que la estructura CMake sea modular, para poder agregar nuevos componentes al proyecto con mínimo esfuerzo.

#### Criterios de Aceptación

1. THE Sistema_Build SHALL organizar cada Módulo (game, net, ui, utils) como una biblioteca estática independiente, donde cada Módulo reside en su propio subdirectorio bajo `src/` con su propio CMakeLists.txt, y compila sin requerir los archivos fuente de otros Módulos.
2. THE Sistema_Build SHALL permitir agregar un nuevo Módulo creando un subdirectorio bajo `src/` que contenga al menos un archivo fuente (.cpp), un header (.hpp), y un CMakeLists.txt que declare una biblioteca estática con `add_library` y exporte sus rutas de inclusión con `target_include_directories(... PUBLIC ...)`, registrándolo en el CMakeLists.txt padre con una sola línea `add_subdirectory(<nombre_modulo>)`.
3. WHEN un nuevo Módulo es registrado mediante `add_subdirectory` en el CMakeLists.txt padre, THE Sistema_Build SHALL hacer accesibles sus headers al Target_Principal a través de `target_link_libraries` sin requerir modificaciones adicionales en el CMakeLists.txt del Target_Principal más allá de agregar el nombre del Módulo a la lista de dependencias.
4. WHEN un Módulo depende de otro Módulo del proyecto, THE Sistema_Build SHALL resolver la dependencia mediante `target_link_libraries` en el CMakeLists.txt del Módulo dependiente, propagando transitivamente las rutas de inclusión del Módulo requerido.

### Requisito 7: Opciones de compilación y flags

**Historia de Usuario:** Como desarrollador, quiero que las opciones de compilación estén correctamente configuradas, para obtener builds optimizados y con buena información de depuración según el tipo de build.

#### Criterios de Aceptación

1. WHILE el tipo de build sea Debug, THE Sistema_Build SHALL compilar con el flag -O0, el flag de símbolos de depuración (-g), -sASSERTIONS=1, y -gsource-map para generar source maps compatibles con Emscripten.
2. WHILE el tipo de build sea Release, THE Sistema_Build SHALL compilar con nivel de optimización -O2 o -O3, y con el flag --closure 1 para habilitar el closure compiler de Emscripten.
3. THE Sistema_Build SHALL habilitar los flags de warnings -Wall y -Wextra para todos los archivos fuente C++ del proyecto (client/src/).
4. IF la compilación produce warnings con -Wall -Wextra habilitados, THEN THE Sistema_Build SHALL reportar los warnings sin interrumpir la compilación (warnings no se tratan como errores).
