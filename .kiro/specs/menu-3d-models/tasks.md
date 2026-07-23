# Implementation Plan: Menu 3D Models

## Overview

This plan implements the 3D background scene for the ChronoClash main menu by building the `GameCamera` orbital controller, the `MenuScene3D` model/lighting manager, creating shader assets, and integrating everything into the existing `MainMenuScreen`. Tasks are ordered so each step builds incrementally on the previous one.

## Tasks

- [x] 1. Implement GameCamera class
  - [x] 1.1 Implement GameCamera in `game/camera.hpp` and `game/camera.cpp`
    - Define `CameraMode` enum (Orbit, Follow, Pan) in `chrono_clash::game` namespace
    - Implement constructor with `initialized_ = false` and safe defaults (distance=10, height=5, fovy=45, target=origin)
    - Implement `init()` with parameter clamping: distance ∈ [1.0, 100.0], height ∈ [-50.0, 50.0], speed ∈ [0.01, 10.0], fovy ∈ [10.0, 120.0]
    - Implement `update(float dt)`: ignore dt ≤ 0, clamp dt > 0.1 to 0.1, advance angle by speed*dt, wrap angle to [0, 2π)
    - Implement `getCamera3D()` returning valid Camera3D with CAMERA_PERSPECTIVE, up=(0,1,0), configured fovy
    - Implement `recomputePosition()`: x = target.x + distance*cos(angle), z = target.z + distance*sin(angle), y = target.y + height
    - Implement `setMode()` and `getMode()` accessors
    - _Requirements: 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 3.1, 3.2, 3.3, 3.4, 3.5_

  - [x] 1.2 Write property test: Camera3D struct validity after init
    - **Property 1: Camera3D struct validity after init**
    - Generate random valid parameters (target, distance, height, speed, fovy within ranges)
    - Assert getCamera3D() returns Camera3D with projection=CAMERA_PERSPECTIVE, up=(0,1,0), correct fovy, position at Euclidean distance ±0.01 from target
    - **Validates: Requirements 1.3, 1.4**

  - [x] 1.3 Write property test: Orbit position invariant
    - **Property 2: Orbit position invariant**
    - Generate random init params and random sequence of valid dt values
    - After each update, assert sqrt((cam.x-target.x)² + (cam.z-target.z)²) == distance ±0.01 AND cam.y == target.y + height AND Camera3D.target == configured target
    - **Validates: Requirements 2.1, 2.2, 2.3, 2.6**

  - [ ]* 1.4 Write property test: Parameter clamping
    - **Property 3: Parameter clamping**
    - Generate random out-of-range parameters and verify internal state matches clamped equivalents
    - **Validates: Requirements 1.5**

  - [ ]* 1.5 Write property test: Update additivity (framerate independence)
    - **Property 4: Update additivity**
    - Generate random positive dt values each ≤ 0.1 where sum ≤ 0.1
    - Assert calling update(dt1) then update(dt2) produces same state as update(dt1+dt2)
    - **Validates: Requirements 3.2**

  - [ ]* 1.6 Write property test: Angle wrapping invariant
    - **Property 5: Angle wrapping invariant**
    - Generate random sequence of updates that cause cumulative angle > 2π
    - Assert getAngle() is always in [0, 2π)
    - **Validates: Requirements 2.5**

  - [ ]* 1.7 Write property test: Delta-time clamping
    - **Property 6: Delta-time clamping**
    - Generate random dt values > 0.1
    - Assert update(large_dt) produces same state as update(0.1)
    - **Validates: Requirements 3.4**

- [ ] 2. Checkpoint - Ensure GameCamera compiles and tests pass
  - Ensure all tests pass, ask the user if questions arise.

- [ ] 3. Create shader assets and directory structure
  - [ ] 3.1 Create lighting shader files in `client/assets/shaders/`
    - Create `assets/shaders/` directory
    - Create `lighting330.vs` — GLSL 330 vertex shader (transforms normals/positions, passes fragPosition and fragNormal to fragment shader)
    - Create `lighting330.fs` — GLSL 330 fragment shader (implements Phong lighting with up to 4 lights, ambient uniform, viewPos uniform)
    - Create `lighting100.vs` — GLSL 100 vertex shader (same logic as 330 but with `attribute`/`varying` syntax, `precision mediump float`)
    - Create `lighting100.fs` — GLSL 100 fragment shader (same logic as 330 but with GLSL ES 2.0 syntax)
    - _Requirements: 5.1, 5.4, 8.5_

  - [ ] 3.2 Create `assets/models/` directory with placeholder files
    - Create `client/assets/models/` directory
    - Add a `.gitkeep` file so the directory is tracked (actual `.glb` files will be added by artists later)
    - _Requirements: 4.1, 9.1, 9.4_

- [ ] 4. Implement MenuScene3D class
  - [ ] 4.1 Create `ui/menu_scene.hpp` with MenuScene3D class declaration
    - Define `ModelTransform` struct (position, rotation, scale) in `chrono_clash::ui` namespace
    - Define `ModelEntry` struct (filePath, transform)
    - Declare `MenuScene3D` class with init/draw/unload methods, MAX_MODELS=10 constant, private model/shader/light members
    - _Requirements: 4.1, 4.3, 4.5, 4.6, 4.7, 5.1_

  - [ ] 4.2 Implement MenuScene3D in `ui/menu_scene.cpp`
    - Include Raylib's `rlights.h` (copy from raylib examples into project or use inline)
    - Implement `loadShader()` with platform selection (`#ifdef PLATFORM_WEB`/`EMSCRIPTEN` for GLSL 100, else GLSL 330)
    - Implement `init()`: cap model count to MAX_MODELS, load shader, create lights (key directional + fill point), load each model or generate fallback cube via `GenMeshCube(1,1,1)`, assign shader to model materials
    - Implement `draw(Vector3 cameraPosition)`: early return if not initialized, set viewPos uniform, iterate models and render with `DrawModelEx()`
    - Implement `unload()`: unload models, unload shader, reset state
    - Implement `loadModelOrFallback()`: attempt `LoadModel()`, check meshCount > 0, generate cube fallback if not
    - _Requirements: 4.1, 4.2, 4.4, 4.5, 4.6, 4.7, 4.8, 5.1, 5.2, 5.3, 5.5, 9.1, 9.2, 9.3_

  - [ ] 4.3 Add `menu_scene.cpp` to `ui/CMakeLists.txt`
    - Add `menu_scene.cpp` to the `add_library(ui STATIC ...)` source list
    - _Requirements: 8.1_

- [ ] 5. Integrate 3D scene into MainMenuScreen
  - [ ] 5.1 Modify `ui/screens.hpp` to add GameCamera and MenuScene3D members
    - Add `#include "game/camera.hpp"` and `#include "menu_scene.hpp"`
    - Add private members: `chrono_clash::game::GameCamera camera3D_` and `chrono_clash::ui::MenuScene3D scene3D_`
    - _Requirements: 6.1, 6.2_

  - [ ] 5.2 Modify `ui/screens.cpp` to integrate 3D rendering
    - In `init()`: after existing resource loading, call `camera3D_.init({0,0,0}, 8.0f, 3.0f, 0.3f, 45.0f)` and `scene3D_.init(MENU_MODELS, 2)` with the default model entries
    - In `update()`: call `camera3D_.update(GetFrameTime())` after existing update logic
    - In `draw()`: restructure to ClearBackground → BeginMode3D(camera3D_.getCamera3D()) → scene3D_.draw(camera position) → EndMode3D() → then existing 2D overlay (title, subtitle, buttons)
    - Add semi-transparent background rectangles behind text/buttons for legibility over 3D
    - In `unload()`: call `scene3D_.unload()` before existing resource cleanup
    - _Requirements: 6.3, 6.4, 6.5, 6.6, 6.7, 6.8, 7.1, 7.2, 7.3, 7.4_

- [ ] 6. Checkpoint - Ensure full build compiles on both native and Emscripten targets
  - Ensure all tests pass, ask the user if questions arise.

- [ ] 7. Copy rlights.h into project
  - [ ] 7.1 Add Raylib's `rlights.h` to the project source tree
    - Copy `rlights.h` from raylib examples (`raylib/examples/shaders/rlights.h`) into `client/src/ui/` or a shared include path
    - Ensure `menu_scene.cpp` can include it without path issues
    - _Requirements: 5.1, 5.2_

- [ ] 8. Final checkpoint - Verify full integration
  - Ensure all tests pass, ask the user if questions arise.

## Notes

- Tasks marked with `*` are optional and can be skipped for faster MVP
- Each task references specific requirements for traceability
- Checkpoints ensure incremental validation
- Property tests validate universal correctness properties on `GameCamera` (pure logic, no GPU needed)
- The `GameCamera` class has no Raylib window dependency for its logic, making it ideal for property-based testing with RapidCheck or similar C++ PBT framework
- Shader files follow Raylib's official lighting example pattern; `rlights.h` provides `CreateLight()` and `UpdateLightValues()`
- Actual `.glb` model assets are not created by this plan — the fallback cube mechanism ensures the menu renders even without art assets
- The `game` library already links to `utils`; the `ui` library already links to `game`, so no additional CMake dependency changes are needed beyond adding `menu_scene.cpp`

## Task Dependency Graph

```json
{
  "waves": [
    { "id": 0, "tasks": ["1.1", "3.1", "3.2"] },
    { "id": 1, "tasks": ["1.2", "1.3", "1.4", "1.5", "1.6", "1.7", "4.1", "7.1"] },
    { "id": 2, "tasks": ["4.2"] },
    { "id": 3, "tasks": ["4.3", "5.1"] },
    { "id": 4, "tasks": ["5.2"] }
  ]
}
```
