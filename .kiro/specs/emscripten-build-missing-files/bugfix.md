# Bugfix Requirements Document

## Introduction

The Emscripten build for ChronoClash fails at the compilation/linking step (`cmake --build` returns exit code 1). The CMake configuration step (`cmake -B`) succeeds, but the subsequent build fails because two Emscripten linker flags in `client/src/CMakeLists.txt` reference files that do not exist in the repository:

1. `--preload-file` references `client/assets/` which does not exist.
2. `--shell-file` references `client/src/shell.html` which does not exist.

This blocks all Emscripten/WebAssembly builds of the project and prevents the integration test (`test_integration_build.cmake`) from passing.

## Bug Analysis

### Current Behavior (Defect)

1.1 WHEN the Emscripten build is invoked with `cmake --build` THEN the system fails with exit code 1 because `--preload-file` references a non-existent directory (`client/assets/`)

1.2 WHEN the Emscripten build is invoked with `cmake --build` THEN the system fails with exit code 1 because `--shell-file` references a non-existent file (`client/src/shell.html`)

1.3 WHEN the integration test `test_integration_build.cmake` is executed THEN the build step reports failure and no output artifacts are generated

### Expected Behavior (Correct)

2.1 WHEN the Emscripten build is invoked with `cmake --build` THEN the system SHALL successfully compile and link, resolving the `--preload-file` flag against an existing `client/assets/` directory

2.2 WHEN the Emscripten build is invoked with `cmake --build` THEN the system SHALL successfully compile and link, resolving the `--shell-file` flag against an existing shell HTML template

2.3 WHEN the integration test `test_integration_build.cmake` is executed THEN the system SHALL produce all expected artifacts (`index.html`, `index.js`, `index.wasm`, `index.data`) with non-zero file sizes

### Unchanged Behavior (Regression Prevention)

3.1 WHEN the CMake configuration step (`cmake -B`) is invoked with the Emscripten toolchain THEN the system SHALL CONTINUE TO complete configuration with exit code 0

3.2 WHEN the project source files (`main.cpp`, module libraries) are compiled THEN the system SHALL CONTINUE TO compile without warnings beyond those already present

3.3 WHEN the Emscripten linker flags `-sUSE_GLFW=3`, `-sASYNCIFY`, `-sALLOW_MEMORY_GROWTH=1`, and `-lwebsocket.js` are applied THEN the system SHALL CONTINUE TO pass those flags to the linker unchanged
