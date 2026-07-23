/**
 * Minimal raylib type stubs for headless property-based testing.
 * Only includes struct definitions and constants needed by GameCamera.
 * No GPU, no window, no rendering — pure math testing.
 */
#ifndef RAYLIB_STUB_H
#define RAYLIB_STUB_H

// Vector3, 3 components
typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;

// Camera3D, defines position/orientation in 3d space
typedef struct Camera3D {
    Vector3 position;
    Vector3 target;
    Vector3 up;
    float fovy;
    int projection;
} Camera3D;

typedef Camera3D Camera;

// Camera projection
typedef enum {
    CAMERA_PERSPECTIVE = 0,
    CAMERA_ORTHOGRAPHIC
} CameraProjection;

#endif // RAYLIB_STUB_H
