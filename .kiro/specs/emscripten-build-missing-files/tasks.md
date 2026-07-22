# Implementation Plan

- [x] 1. Write bug condition exploration test
  - **Property 1: Bug Condition** - Emscripten Build Fails Without Missing Files
  - **CRITICAL**: This test MUST FAIL on unfixed code - failure confirms the bug exists
  - **DO NOT attempt to fix the test or the code when it fails**
  - **NOTE**: This test encodes the expected behavior - it will validate the fix when it passes after implementation
  - **GOAL**: Surface counterexamples that demonstrate the build fails due to missing files
  - **Scoped PBT Approach**: Since this is a deterministic build failure, scope to the concrete case: run `cmake -P client/tests/test_integration_build.cmake` and assert exit code 0
  - Verify `client/assets/` does NOT exist (precondition for bug)
  - Verify `client/src/shell.html` does NOT exist (precondition for bug)
  - Run the integration test and confirm it fails at the build step (exit code != 0)
  - Document counterexamples: build error output referencing missing preload-file or shell-file paths
  - **EXPECTED OUTCOME**: Test FAILS (this is correct - it proves the bug exists)
  - Mark task complete when test is run and failure is documented
  - _Requirements: 1.1, 1.2, 1.3_

- [x] 2. Write preservation property tests (BEFORE implementing fix)
  - **Property 2: Preservation** - CMake Configuration Succeeds
  - **IMPORTANT**: Follow observation-first methodology
  - Observe: Run `cmake -B build_test -DCMAKE_TOOLCHAIN_FILE=<emscripten_toolchain>` on unfixed code from `client/` directory
  - Observe: Configuration step completes with exit code 0 (this is the existing behavior to preserve)
  - Observe: The generated build files contain the expected linker flags (`-sUSE_GLFW=3`, `-sASYNCIFY`, `-sALLOW_MEMORY_GROWTH=1`, `-lwebsocket.js`)
  - Write a test script that runs `cmake -B` and asserts exit code 0
  - Verify test passes on UNFIXED code (configuration works even without the missing files)
  - **EXPECTED OUTCOME**: Tests PASS (this confirms baseline behavior to preserve)
  - Mark task complete when tests are written, run, and passing on unfixed code
  - _Requirements: 3.1, 3.2, 3.3_

- [x] 3. Fix for missing files causing Emscripten build failure

  - [x] 3.1 Create `client/assets/.gitkeep` to establish the assets directory
    - Create an empty file at `client/assets/.gitkeep`
    - This ensures the `client/assets/` directory exists in the repository
    - The `--preload-file` flag will resolve against this directory (Emscripten handles empty directories gracefully)
    - _Bug_Condition: isBugCondition(input) where NOT EXISTS "client/assets/"_
    - _Expected_Behavior: cmake --build completes with exit code 0_
    - _Preservation: CMake configuration and other linker flags unchanged_
    - _Requirements: 2.1_

  - [x] 3.2 Create `client/src/shell.html` Emscripten shell template
    - Create a valid Emscripten HTML shell template based on raylib's reference at `client/lib/raylib/src/shell.html`
    - MUST include `{{{ SCRIPT }}}` placeholder (required by Emscripten)
    - MUST include `<canvas>` element with id `canvas` (required by raylib web backend)
    - MUST include `Module` JavaScript object with `canvas`, `setStatus`, and print/error handlers
    - Customize title and branding for ChronoClash (remove raylib-specific branding/links)
    - _Bug_Condition: isBugCondition(input) where NOT EXISTS "client/src/shell.html"_
    - _Expected_Behavior: cmake --build resolves --shell-file successfully_
    - _Preservation: No changes to CMakeLists.txt or source code_
    - _Requirements: 2.2_

  - [x] 3.3 Verify bug condition exploration test now passes
    - **Property 1: Expected Behavior** - Emscripten Build Succeeds
    - **IMPORTANT**: Re-run the SAME test from task 1 - do NOT write a new test
    - The test from task 1 encodes the expected behavior (build succeeds with exit code 0)
    - Run `cmake -P client/tests/test_integration_build.cmake`
    - **EXPECTED OUTCOME**: Test PASSES (confirms build now succeeds and produces index.html, index.js, index.wasm, index.data)
    - _Requirements: 2.1, 2.2, 2.3_

  - [x] 3.4 Verify preservation tests still pass
    - **Property 2: Preservation** - CMake Configuration Still Succeeds
    - **IMPORTANT**: Re-run the SAME tests from task 2 - do NOT write new tests
    - Run preservation test (cmake -B configuration step)
    - **EXPECTED OUTCOME**: Tests PASS (confirms configuration still works, no regressions)
    - Confirm all linker flags are unchanged in the generated build system

- [x] 4. Checkpoint - Ensure all tests pass
  - Run full integration test: `cmake -P client/tests/test_integration_build.cmake`
  - Verify all 4 artifacts produced (index.html, index.js, index.wasm, index.data) with non-zero sizes
  - Verify no new warnings introduced
  - Ensure all tests pass, ask the user if questions arise.
