#pragma once

#include "raylib.h"

namespace chrono_clash::game {

enum class CameraMode { Orbit, Follow, Pan };

class GameCamera {
public:
    GameCamera();

    /// Initialize with orbit/follow parameters. Clamps out-of-range values.
    void init(Vector3 target, float distance, float height, float speed, float fovy);

    /// Advance camera state by dt seconds (clamped to [0, 0.1]).
    void update(float dt);

    /// Get the current Raylib Camera3D (safe defaults if init() not called).
    Camera3D getCamera3D() const;

    /// Change active mode.
    void setMode(CameraMode mode);
    CameraMode getMode() const;

    /// Read current orbital angle (radians, [0, 2π)).
    float getAngle() const;

private:
    Camera3D camera_;
    CameraMode mode_;
    Vector3 target_;
    float distance_;
    float height_;
    float speed_;       // radians per second
    float angle_;       // current orbital angle in [0, 2π)
    bool initialized_;

    void recomputePosition();
};

} // namespace chrono_clash::game
