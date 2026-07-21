# Emscripten Build Missing Files Bugfix Design

## Overview

The Emscripten/WebAssembly build of ChronoClash fails at link time because two files referenced by linker flags in `client/src/CMakeLists.txt` do not exist in the repository:

1. `client/assets/` — referenced by `--preload-file`
2. `client/src/shell.html` — referenced by `--shell-file`

The fix is straightforward: create both missing files so the existing CMake configuration resolves correctly. No changes to build logic or CMakeLists.txt are required.

## Glossary

- **Bug_Condition (C)**: The condition that triggers the bug — the Emscripten build step (`cmake --build`) is invoked while `client/assets/` or `client/src/shell.html` do not exist on disk
- **Property (P)**: The desired behavior — `cmake --build` completes with exit code 0 and produces all expected artifacts (`index.html`, `index.js`, `index.wasm`, `index.data`)
- **Preservation**: The CMake configuration step, compilation of source files, and all other linker flags must remain unchanged by the fix
- **`--preload-file`**: Emscripten linker flag that bundles a directory's contents into a `.data` file for the virtual filesystem
- **`--shell-file`**: Emscripten linker flag that specifies an HTML template wrapping the generated JavaScript/WebAssembly output

## Bug Details

### Bug Condition

The bug manifests when the Emscripten build step (`cmake --build`) is invoked. The linker (`emcc`) fails because it cannot resolve two file references declared in `client/src/CMakeLists.txt`:

- `--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/../assets@/assets` → resolves to `client/assets/` which does not exist
- `--shell-file=${CMAKE_CURRENT_SOURCE_DIR}/shell.html` → resolves to `client/src/shell.html` which does not exist

**Formal Specification:**
```
FUNCTION isBugCondition(input)
  INPUT: input of type BuildInvocation
  OUTPUT: boolean

  RETURN input.command = "cmake --build"
         AND input.toolchain = "Emscripten"
         AND (NOT EXISTS "client/assets/" OR NOT EXISTS "client/src/shell.html")
END FUNCTION
```

### Examples

- `cmake --build build_web` → fails with exit code 1 because `client/assets/` directory is missing
- `cmake --build build_web` → fails with exit code 1 because `client/src/shell.html` file is missing
- After creating both files, `cmake --build build_web` → succeeds with exit code 0
- With only `client/assets/` created (but not `shell.html`) → still fails

## Expected Behavior

### Preservation Requirements

**Unchanged Behaviors:**
- CMake configuration step (`cmake -B`) must continue to complete with exit code 0
- Compilation of C++ source files (`main.cpp`, module libraries) must produce no new warnings
- Linker flags `-sUSE_GLFW=3`, `-sASYNCIFY`, `-sALLOW_MEMORY_GROWTH=1`, and `-lwebsocket.js` must continue to pass to the linker unchanged
- Debug/Release-specific flags (`-sASSERTIONS=1`, `-gsource-map`, `--closure=1`) must remain unchanged

**Scope:**
All inputs that do NOT involve the two missing files should be completely unaffected by this fix. This includes:
- The CMake configuration phase (no changes to CMakeLists.txt)
- Source code compilation (no changes to .cpp files)
- Other linker flags (only the referenced files change, not the flags themselves)
- Non-Emscripten builds (native builds are unaffected since the flags are guarded by `if(EMSCRIPTEN)`)

## Hypothesized Root Cause

Based on the bug description, the root cause is clear:

1. **Missing `client/assets/` directory**: The `--preload-file` flag was added to CMakeLists.txt anticipating an assets directory that was never committed to the repository. Emscripten's file packager (`file_packager.py`) fails when the source path does not exist.

2. **Missing `client/src/shell.html`**: The `--shell-file` flag references a custom HTML shell template that was never created. Emscripten requires this file to exist at link time to generate the final HTML output.

Both are simply files that should have been committed alongside the CMakeLists.txt configuration.

## Correctness Properties

Property 1: Bug Condition - Emscripten Build Succeeds With Missing Files Created

_For any_ build invocation where the Emscripten toolchain is used and the project is configured with the existing CMakeLists.txt, the build step (`cmake --build`) SHALL complete with exit code 0 and produce artifacts `index.html`, `index.js`, `index.wasm`, and `index.data` with non-zero file sizes.

**Validates: Requirements 2.1, 2.2, 2.3**

Property 2: Preservation - CMake Configuration and Linker Flags Unchanged

_For any_ build invocation that does NOT involve the two missing files (i.e., the configuration step, source compilation, and other linker flags), the fixed repository SHALL produce exactly the same behavior as would be expected — configuration completes with exit code 0, no new compilation warnings, and all other linker flags pass unchanged.

**Validates: Requirements 3.1, 3.2, 3.3**

## Fix Implementation

### Changes Required

**File**: `client/assets/.gitkeep`

**Purpose**: Create the assets directory with a `.gitkeep` placeholder so it persists in git (git does not track empty directories).

**Specific Changes**:
1. **Create `client/assets/.gitkeep`**: An empty file that ensures the `client/assets/` directory exists in the repository. The `--preload-file` flag will resolve successfully against this directory (Emscripten handles empty directories gracefully, producing a minimal `.data` file).

---

**File**: `client/src/shell.html`

**Purpose**: Create a valid Emscripten HTML shell template for the ChronoClash project.

**Specific Changes**:
2. **Create `client/src/shell.html`**: A proper Emscripten shell HTML template based on raylib's reference template at `client/lib/raylib/src/shell.html`. The template must:
   - Include the `{{{ SCRIPT }}}` placeholder (required by Emscripten to inject the generated JS)
   - Include a `<canvas>` element with id `canvas` (required by raylib's web backend)
   - Include the `Module` JavaScript object with `canvas`, `setStatus`, and print/error handlers
   - Be customized for ChronoClash (title, branding) rather than generic raylib branding

## Testing Strategy

### Validation Approach

The testing strategy follows a two-phase approach: first, confirm the bug exists on unfixed code by running the integration test, then verify the fix resolves it and preserves configuration behavior.

### Exploratory Bug Condition Checking

**Goal**: Surface counterexamples that demonstrate the bug BEFORE implementing the fix. Confirm that the build fails due to missing files.

**Test Plan**: Run the existing integration test `cmake -P client/tests/test_integration_build.cmake` on the UNFIXED code. Observe that the build step fails with exit code 1.

**Test Cases**:
1. **Full Integration Test**: Run `test_integration_build.cmake` — expect configuration passes but build fails (will fail on unfixed code)
2. **Missing Assets Check**: Verify `client/assets/` does not exist (confirms precondition)
3. **Missing Shell Check**: Verify `client/src/shell.html` does not exist (confirms precondition)

**Expected Counterexamples**:
- Build step returns exit code 1
- Error output references missing preload-file path or missing shell-file path

### Fix Checking

**Goal**: Verify that for all inputs where the bug condition holds, the fixed repository produces a successful build.

**Pseudocode:**
```
FOR ALL input WHERE isBugCondition(input) DO
  result := cmake_build(input)
  ASSERT result.exitCode = 0
  ASSERT EXISTS "index.html" AND SIZE("index.html") > 0
  ASSERT EXISTS "index.js" AND SIZE("index.js") > 0
  ASSERT EXISTS "index.wasm" AND SIZE("index.wasm") > 0
  ASSERT EXISTS "index.data" AND SIZE("index.data") > 0
END FOR
```

### Preservation Checking

**Goal**: Verify that for all inputs where the bug condition does NOT hold, the fixed repository behaves identically to the original.

**Pseudocode:**
```
FOR ALL input WHERE NOT isBugCondition(input) DO
  ASSERT cmake_configure(input).exitCode = 0
  ASSERT compilation_warnings(input) = original_warnings(input)
  ASSERT linker_flags(input) = original_linker_flags(input)
END FOR
```

**Testing Approach**: Since this bugfix only adds files (no code changes), preservation is inherently guaranteed for non-build inputs. The key preservation check is that `cmake -B` still succeeds with exit code 0 after the files are added.

**Test Plan**: Run `cmake -B` on the fixed code and verify it completes with exit code 0. Inspect the generated build system to confirm linker flags are unchanged.

**Test Cases**:
1. **Configuration Preservation**: Run `cmake -B` with Emscripten toolchain — expect exit code 0 (same as before fix)
2. **Linker Flags Preservation**: Inspect the build system files to confirm `-sUSE_GLFW=3`, `-sASYNCIFY`, `-sALLOW_MEMORY_GROWTH=1`, `-lwebsocket.js` are present

### Unit Tests

- Verify `client/assets/.gitkeep` exists and directory is tracked
- Verify `client/src/shell.html` exists and contains required Emscripten placeholders (`{{{ SCRIPT }}}`, `canvas` element, `Module` object)
- Verify shell.html is valid HTML

### Property-Based Tests

- For this bugfix, property-based testing applies primarily to the integration test: the build should succeed for any valid Emscripten toolchain configuration when the files exist
- Preservation: `cmake -B` should succeed regardless of whether the assets directory is empty or populated

### Integration Tests

- Run full `test_integration_build.cmake` — all steps should pass
- Verify all four artifacts are produced with non-zero sizes
- Verify the generated `index.html` uses the custom shell template content
