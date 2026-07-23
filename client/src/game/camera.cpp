#include "camera.hpp"

#include <cmath>

namespace chrono_clash::game {

namespace {

constexpr float PI_2 = 2.0f * 3.14159265358979323846f;

float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

} // anonymous namespace

GameCamera::GameCamera()
    : mode_(CameraMode::Orbit)
    , target_{0.0f, 0.0f, 0.0f}
    , distance_(10.0f)
    , height_(5.0f)
    , speed_(0.0f)
    , angle_(0.0f)
    , initialized_(false)
{
    camera_ = {};
    camera_.position = {target_.x + distance_ * std::cos(angle_),
                        target_.y + height_,
                        target_.z + distance_ * std::sin(angle_)};
    camera_.target = target_;
    camera_.up = {0.0f, 1.0f, 0.0f};
    camera_.fovy = 45.0f;
    camera_.projection = CAMERA_PERSPECTIVE;
}

void GameCamera::init(Vector3 target, float distance, float height, float speed, float fovy) {
    target_ = target;
    distance_ = clamp(distance, 1.0f, 100.0f);
    height_ = clamp(height, -50.0f, 50.0f);
    speed_ = clamp(speed, 0.01f, 10.0f);
    angle_ = 0.0f;

    camera_.fovy = clamp(fovy, 10.0f, 120.0f);
    camera_.projection = CAMERA_PERSPECTIVE;
    camera_.up = {0.0f, 1.0f, 0.0f};
    camera_.target = target_;

    initialized_ = true;

    recomputePosition();
}

void GameCamera::update(float dt) {
    if (dt <= 0.0f) return;

    if (dt > 0.1f) {
        dt = 0.1f;
    }

    angle_ += speed_ * dt;

    while (angle_ >= PI_2) {
        angle_ -= PI_2;
    }

    recomputePosition();
}

Camera3D GameCamera::getCamera3D() const {
    return camera_;
}

void GameCamera::setMode(CameraMode mode) {
    mode_ = mode;
}

CameraMode GameCamera::getMode() const {
    return mode_;
}

float GameCamera::getAngle() const {
    return angle_;
}

void GameCamera::recomputePosition() {
    camera_.position.x = target_.x + distance_ * std::cos(angle_);
    camera_.position.z = target_.z + distance_ * std::sin(angle_);
    camera_.position.y = target_.y + height_;
    camera_.target = target_;
    camera_.up = {0.0f, 1.0f, 0.0f};
    camera_.fovy = camera_.fovy; // preserve current fovy
    camera_.projection = CAMERA_PERSPECTIVE;
}

} // namespace chrono_clash::game
