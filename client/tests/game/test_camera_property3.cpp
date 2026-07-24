/**
 * Property Test: Parameter clamping
 *
 * Validates: Requirements 1.5
 *
 * Property 3: For any input parameters where distance, height, speed, or fovy
 * fall outside their valid ranges, calling init() SHALL produce an internal
 * state equivalent to calling init() with those parameters clamped to
 * [1.0, 100.0], [-50.0, 50.0], [0.01, 10.0], and [10.0, 120.0] respectively.
 */

#include "camera.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <random>

namespace {

constexpr int NUM_ITERATIONS = 200;
constexpr float TOLERANCE = 0.01f;

// Random float in [min, max]
float randomFloat(std::mt19937& rng, float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

bool approxEqual(float a, float b, float tol = TOLERANCE) {
    return std::fabs(a - b) <= tol;
}

// Generate a random out-of-range value for a parameter.
// Randomly decides if the value goes below min or above max.
float randomOutOfRange(std::mt19937& rng, float validMin, float validMax) {
    std::uniform_int_distribution<int> coinFlip(0, 1);
    if (coinFlip(rng) == 0) {
        // Below minimum: generate in [validMin - 500, validMin - 0.01]
        return randomFloat(rng, validMin - 500.0f, validMin - 0.01f);
    } else {
        // Above maximum: generate in [validMax + 0.01, validMax + 500]
        return randomFloat(rng, validMax + 0.01f, validMax + 500.0f);
    }
}

float clampValue(float v, float min, float max) {
    return std::max(min, std::min(max, v));
}

bool camera3DEqual(const Camera3D& a, const Camera3D& b) {
    if (!approxEqual(a.position.x, b.position.x)) return false;
    if (!approxEqual(a.position.y, b.position.y)) return false;
    if (!approxEqual(a.position.z, b.position.z)) return false;
    if (!approxEqual(a.target.x, b.target.x)) return false;
    if (!approxEqual(a.target.y, b.target.y)) return false;
    if (!approxEqual(a.target.z, b.target.z)) return false;
    if (!approxEqual(a.up.x, b.up.x)) return false;
    if (!approxEqual(a.up.y, b.up.y)) return false;
    if (!approxEqual(a.up.z, b.up.z)) return false;
    if (!approxEqual(a.fovy, b.fovy)) return false;
    if (a.projection != b.projection) return false;
    return true;
}

} // anonymous namespace

int main() {
    std::mt19937 rng(42); // fixed seed for reproducibility

    int passed = 0;
    int failed = 0;

    std::printf("Property Test 3: Parameter clamping\n");
    std::printf("Running %d iterations...\n", NUM_ITERATIONS);

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        // Generate a random target (any Vector3, not clamped)
        Vector3 target{
            randomFloat(rng, -100.0f, 100.0f),
            randomFloat(rng, -100.0f, 100.0f),
            randomFloat(rng, -100.0f, 100.0f)
        };

        // Generate out-of-range parameters
        float rawDistance = randomOutOfRange(rng, 1.0f, 100.0f);
        float rawHeight = randomOutOfRange(rng, -50.0f, 50.0f);
        float rawSpeed = randomOutOfRange(rng, 0.01f, 10.0f);
        float rawFovy = randomOutOfRange(rng, 10.0f, 120.0f);

        // Manually compute clamped values
        float clampedDistance = clampValue(rawDistance, 1.0f, 100.0f);
        float clampedHeight = clampValue(rawHeight, -50.0f, 50.0f);
        float clampedSpeed = clampValue(rawSpeed, 0.01f, 10.0f);
        float clampedFovy = clampValue(rawFovy, 10.0f, 120.0f);

        // Create camera initialized with out-of-range params
        chrono_clash::game::GameCamera camRaw;
        camRaw.init(target, rawDistance, rawHeight, rawSpeed, rawFovy);

        // Create camera initialized with pre-clamped params
        chrono_clash::game::GameCamera camClamped;
        camClamped.init(target, clampedDistance, clampedHeight, clampedSpeed, clampedFovy);

        bool iterPassed = true;

        // Compare Camera3D structs
        Camera3D c3dRaw = camRaw.getCamera3D();
        Camera3D c3dClamped = camClamped.getCamera3D();

        if (!camera3DEqual(c3dRaw, c3dClamped)) {
            std::printf("  FAIL [iter %d]: Camera3D mismatch between raw and clamped init\n", i);
            std::printf("    Raw params: distance=%f, height=%f, speed=%f, fovy=%f\n",
                        rawDistance, rawHeight, rawSpeed, rawFovy);
            std::printf("    Clamped params: distance=%f, height=%f, speed=%f, fovy=%f\n",
                        clampedDistance, clampedHeight, clampedSpeed, clampedFovy);
            std::printf("    Raw Camera3D: pos=(%f,%f,%f) target=(%f,%f,%f) fovy=%f proj=%d\n",
                        c3dRaw.position.x, c3dRaw.position.y, c3dRaw.position.z,
                        c3dRaw.target.x, c3dRaw.target.y, c3dRaw.target.z,
                        c3dRaw.fovy, c3dRaw.projection);
            std::printf("    Clamped Camera3D: pos=(%f,%f,%f) target=(%f,%f,%f) fovy=%f proj=%d\n",
                        c3dClamped.position.x, c3dClamped.position.y, c3dClamped.position.z,
                        c3dClamped.target.x, c3dClamped.target.y, c3dClamped.target.z,
                        c3dClamped.fovy, c3dClamped.projection);
            iterPassed = false;
        }

        // Compare angle
        float angleRaw = camRaw.getAngle();
        float angleClamped = camClamped.getAngle();
        if (!approxEqual(angleRaw, angleClamped)) {
            std::printf("  FAIL [iter %d]: Angle mismatch: raw=%f, clamped=%f\n",
                        i, angleRaw, angleClamped);
            iterPassed = false;
        }

        // Compare mode
        if (camRaw.getMode() != camClamped.getMode()) {
            std::printf("  FAIL [iter %d]: Mode mismatch\n", i);
            iterPassed = false;
        }

        if (iterPassed) {
            ++passed;
        } else {
            ++failed;
        }
    }

    std::printf("\nResults: %d/%d passed, %d failed\n", passed, NUM_ITERATIONS, failed);

    if (failed > 0) {
        std::printf("PROPERTY TEST FAILED\n");
        return 1;
    }

    std::printf("PROPERTY TEST PASSED\n");
    return 0;
}
