# Requirements Document

## Introduction

This feature transforms the ChronoClash main menu from a purely 2D screen into a 3D scene with an orbital camera panning smoothly around a character/pedestal model, while keeping the existing 2D UI overlay (title, buttons) rendered on top. It also introduces a reusable `GameCamera` class that supports multiple camera modes for use in both the menu and future gameplay.

## Glossary

- **GameCamera**: A reusable C++ class in namespace `chrono_clash::game` that encapsulates Raylib's `Camera3D` and provides configurable camera behaviors (orbit, follow, pan).
- **CameraMode**: An enumeration defining the available camera behavior modes: `Orbit`, `Follow`, `Pan`.
- **MenuScene3D**: A C++ class in namespace `chrono_clash::ui` responsible for loading, managing, and rendering 3D models and lighting for the main menu background scene.
- **MainMenuScreen**: The existing UI class that manages the main menu lifecycle (init, update, draw, unload) and currently renders a 2D-only menu.
- **Orbit_Mode**: A camera behavior where the camera rotates smoothly around a focal point at a configurable distance, height, and angular speed.
- **Model_Transform**: A data structure holding position (Vector3), rotation (Vector3), and scale (float) for a 3D model in the scene.

## Requirements

### Requirement 1: GameCamera Initialization and Configuration

**User Story:** As a developer, I want a reusable camera class with configurable parameters, so that I can set up different camera behaviors for the menu and gameplay without duplicating code.

#### Acceptance Criteria

1. THE GameCamera SHALL provide an `init()` method that accepts target position (Vector3), distance (float, range 1.0 to 100.0 world units), height (float, range -50.0 to 50.0 world units), speed (float, range 0.01 to 10.0 units per second), and field of view (float, range 10.0 to 120.0 degrees) parameters.
2. THE GameCamera SHALL store a CameraMode value (one of Orbit, Follow, or Pan) that determines the active camera behavior, defaulting to Orbit when no mode is explicitly set.
3. THE GameCamera SHALL expose a `getCamera3D()` method that returns a valid Raylib `Camera3D` struct with projection type CAMERA_PERSPECTIVE, the configured fovy, a non-zero up vector of (0, 1, 0), and position and target fields consistent with the current distance, height, and target position.
4. WHEN `init()` is called with parameters within valid ranges, THE GameCamera SHALL position the internal Camera3D at the specified distance and height relative to the target, such that the Euclidean distance from camera position to target position equals the configured distance value (tolerance ±0.01 world units) and the camera position y-coordinate equals target y-coordinate plus the configured height value.
5. IF `init()` is called with any parameter outside its valid range, THEN THE GameCamera SHALL clamp the parameter to the nearest valid bound and initialize successfully using the clamped values.
6. WHEN `getCamera3D()` is called before `init()` has been called, THE GameCamera SHALL return a Camera3D struct with default safe values (distance 10.0, height 5.0, fovy 45.0, target at origin, mode Orbit) rather than uninitialized memory.

### Requirement 2: Orbit Camera Mode

**User Story:** As a player, I want the menu camera to pan smoothly around the scene, so that the 3D background feels dynamic and polished.

#### Acceptance Criteria

1. WHILE CameraMode is set to Orbit, THE GameCamera SHALL compute the camera position as `x = target.x + distance * cos(angle)`, `z = target.z + distance * sin(angle)`, `y = target.y + height`, where `distance`, `height`, and `target` are the configured orbit parameters.
2. WHILE CameraMode is set to Orbit, THE GameCamera SHALL maintain the radial distance from the target point equal to the configured `distance` value, with no deviation across frames.
3. WHILE CameraMode is set to Orbit, THE GameCamera SHALL maintain the camera Y-coordinate at exactly `target.y + height`, where `height` is the configured orbit height offset.
4. WHEN `update(dt)` is called in Orbit mode, THE GameCamera SHALL advance the orbital angle by exactly `speed * dt` radians, where `speed` is the configured angular speed in radians per second.
5. WHEN the orbital angle exceeds 2π radians, THE GameCamera SHALL wrap the angle back into the range [0, 2π) by subtracting 2π, preventing unbounded angle growth.
6. WHILE CameraMode is set to Orbit, THE GameCamera SHALL set the camera look-at target to the configured target point every frame, ensuring the camera always faces the focal point regardless of orbital position.
7. WHEN `update(dt)` is called with `dt` equal to zero, THE GameCamera SHALL not change the orbital angle or camera position.

### Requirement 3: GameCamera Update Interface

**User Story:** As a developer, I want the camera to update its state each frame based on elapsed time, so that camera movement is framerate-independent.

#### Acceptance Criteria

1. THE GameCamera SHALL provide an `update(float dt)` method that advances the camera state by `dt` seconds.
2. WHEN `update(dt)` is called with a positive delta time, THE GameCamera SHALL advance the camera angular position as a linear function of accumulated `dt`, such that calling `update(dt)` N times with values dt1, dt2, ..., dtN produces the same camera state as a single call `update(dt1 + dt2 + ... + dtN)`.
3. IF `update(dt)` is called with a zero or negative delta time, THEN THE GameCamera SHALL leave the camera position, target, and all internal state unchanged compared to their values before the call.
4. IF `update(dt)` is called with a `dt` value greater than 0.1 seconds, THEN THE GameCamera SHALL clamp `dt` to 0.1 seconds before applying any state changes.
5. WHEN `update(dt)` is called, THE GameCamera SHALL update the internal Raylib `Camera3D` struct such that consecutive frames with `dt` values between 0.0001 and 0.1 seconds produce angular position changes proportional to `dt` with no discontinuities greater than the change expected from a single frame at `dt = 0.1`.

### Requirement 4: MenuScene3D Model Management

**User Story:** As a developer, I want a class that manages 3D models for the menu scene, so that loading, transforming, and rendering menu models is encapsulated in one place.

#### Acceptance Criteria

1. THE MenuScene3D SHALL provide an `init()` method that loads 3D models specified by an internal list of file paths relative to the `assets/models/` directory, where each entry maps a model identifier to its `.glb` filename and a default Model_Transform.
2. IF a model file does not exist at the expected path, THEN THE MenuScene3D SHALL generate a unit-size (1.0 × 1.0 × 1.0) cube mesh as a fallback placeholder and assign it the same Model_Transform configured for the missing model.
3. THE MenuScene3D SHALL store each model with its associated Model_Transform (position as Vector3, rotation as Vector3 in Euler angles, and scale as float).
4. THE MenuScene3D SHALL provide an `unload()` method that releases all loaded model and mesh resources and resets the internal state so that `draw()` called after `unload()` renders nothing.
5. THE MenuScene3D SHALL provide a `draw()` method that renders all managed models at their configured transforms using Raylib's `DrawModel()` or equivalent.
6. IF `draw()` is called before `init()` has been called, THEN THE MenuScene3D SHALL perform no rendering and return without error.
7. THE MenuScene3D SHALL support a maximum of 10 managed models.
8. WHEN `init()` completes, THE MenuScene3D SHALL have loaded or generated fallback geometry for every entry in its internal model list, with zero entries left in an uninitialized state.

### Requirement 5: MenuScene3D Lighting

**User Story:** As a player, I want the 3D menu scene to have basic lighting, so that models are visible and visually appealing.

#### Acceptance Criteria

1. THE MenuScene3D SHALL load a lighting shader during `init()` and assign it to all managed models so that lighting calculations are performed during rendering.
2. THE MenuScene3D SHALL configure at least one directional or point light source during `init()` with a defined position or direction and color value.
3. WHEN `draw()` is called, THE MenuScene3D SHALL render models with the lighting shader active so that surfaces facing the light source appear brighter than surfaces facing away, producing non-uniform shading across model geometry.
4. THE MenuScene3D SHALL use a shader compatible with both the native build (OpenGL 3.3 / GLSL 330) and the web build (OpenGL ES 2.0 / GLSL 100) targets.
5. WHEN `unload()` is called, THE MenuScene3D SHALL unload the lighting shader resource in addition to model and mesh resources.

### Requirement 6: Integration of 3D Scene into MainMenuScreen

**User Story:** As a player, I want to see a 3D scene as the background of the main menu with the existing UI rendered on top, so that the menu feels immersive while remaining functional.

#### Acceptance Criteria

1. THE MainMenuScreen SHALL hold a GameCamera instance configured in Orbit mode targeting the origin Vector3{0, 0, 0} of the menu scene coordinate space.
2. THE MainMenuScreen SHALL hold a MenuScene3D instance for managing the 3D background.
3. WHEN `init()` is called on MainMenuScreen, THE MainMenuScreen SHALL initialize the GameCamera, the MenuScene3D, and the existing 2D resources (Font, Music, Sound) so that all coexist as loaded members simultaneously.
4. IF `update()` or `draw()` is called on MainMenuScreen before `init()` has completed, THEN THE MainMenuScreen SHALL return immediately without modifying state or issuing draw calls.
5. WHEN `update()` is called on MainMenuScreen, THE MainMenuScreen SHALL call `GameCamera.update()` passing `GetFrameTime()` as the delta time parameter.
6. WHEN `draw()` is called on MainMenuScreen, THE MainMenuScreen SHALL clear the background, call `BeginMode3D()` with the `Camera3D` returned by `GameCamera.getCamera3D()`, invoke `MenuScene3D.draw()` within the 3D block, call `EndMode3D()`, and then render the 2D UI overlay on top.
7. THE MainMenuScreen SHALL render the 2D UI elements (title, subtitle, buttons) after `EndMode3D` so they appear on top of the 3D scene.
8. WHEN `unload()` is called on MainMenuScreen, THE MainMenuScreen SHALL call `MenuScene3D.unload()` to free 3D resources while continuing to unload existing 2D resources (Font, Music, Sound) as before.

### Requirement 7: Render Order Correctness

**User Story:** As a player, I want the 2D menu buttons and text to always be visible and interactive above the 3D background, so that the menu remains usable.

#### Acceptance Criteria

1. THE MainMenuScreen SHALL render all 3D scene content within a BeginMode3D/EndMode3D block, and render all 2D UI content (title text, subtitle, buttons) after EndMode3D is called, within each frame.
2. WHILE the 3D background is rendering, THE MainMenuScreen SHALL preserve hover detection via CheckCollisionPointRec and click handling via IsMouseButtonReleased for all menu buttons using screen-space mouse coordinates.
3. THE MainMenuScreen SHALL draw semi-transparent background rectangles behind button text and title text so that 2D UI elements remain legible when overlaid on the 3D scene.
4. THE MainMenuScreen SHALL maintain existing audio behavior (looping music playback via UpdateMusicStream, button hover sound, button press sound) without changes.

### Requirement 8: Build Compatibility

**User Story:** As a developer, I want the 3D menu feature to compile and run on both native (desktop) and web (Emscripten) targets, so that the game works on all supported platforms.

#### Acceptance Criteria

1. THE MenuScene3D source file SHALL be listed in the `add_library(ui STATIC ...)` command in `ui/CMakeLists.txt` so the UI library compiles it.
2. THE GameCamera source file SHALL remain listed in the `add_library(game STATIC ...)` command in `game/CMakeLists.txt` as part of the game library.
3. WHEN the project is built with Emscripten using `emcmake cmake` followed by `make`, THE system SHALL complete compilation with zero errors and zero linker unresolved-symbol errors, and the resulting `index.html` SHALL render the 3D menu scene in a browser without WebGL context errors.
4. WHEN the project is built natively using `cmake` followed by `make`, THE system SHALL complete compilation with zero errors and zero linker unresolved-symbol errors, and the resulting executable SHALL render the 3D menu scene in the desktop window without segmentation faults on launch.
5. THE MenuScene3D implementation SHALL use only Raylib 5.x API functions that are supported on both native (OpenGL 3.3+) and web (OpenGL ES 2.0 via Emscripten) targets, avoiding any platform-specific preprocessor branches unless guarded by `#ifdef PLATFORM_WEB` or `#ifdef EMSCRIPTEN`.
6. IF the `assets` directory referenced by `--preload-file` is missing any model or texture file required by MenuScene3D, THEN the Emscripten build SHALL either fail at link time with a missing preload error or the application SHALL display the scene using Raylib's default fallback geometry without crashing.

### Requirement 9: Asset Loading for Production Models

**User Story:** As a developer, I want to load real `.glb` model files when available, so that the menu displays final art assets.

#### Acceptance Criteria

1. WHEN `init()` is called with a list of model descriptors (each containing a file path relative to `assets/models/` and an associated Model_Transform), THE MenuScene3D SHALL attempt to load each `.glb` file using Raylib's `LoadModel()` function.
2. IF `LoadModel()` returns a model with `meshCount` equal to zero for a given file path, THEN THE MenuScene3D SHALL fall back to generating a primitive placeholder as defined in Requirement 4 criterion 2.
3. THE MenuScene3D SHALL accept Model_Transform values (position as Vector3, rotation as Vector3 in degrees, and scale as a float greater than zero) as parameters passed at init time, allowing adjustment of each model's transform without modifying loading logic.
4. WHEN built with Emscripten, THE system SHALL include the `assets/models/` directory in the preloaded asset bundle via the existing `--preload-file` configuration that recursively bundles `client/assets/` to the `/assets` virtual path.
