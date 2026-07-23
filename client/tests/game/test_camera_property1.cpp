/**
 * Property Test: Camera3D struct validity after init
 *
 * Validates: Requirements 1.3, 1.4
 *
 * Property 1: For any valid combination of target, distance, height, speed,
 * and fovy, calling init() followed by getCamera3D() SHALL return a Camera3D
 * with projection=CAMERA_PERSPECTIVE, up=(0,1,0), fovy equal to the input fovy,
 * and camera position at horizontal (xz-plane) Euclidean distance equal to
 * the configured distance from the target (±0.01).
 */

#include "camera.hpp"

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

} // anonymous namespace

int main() {
    std::mt19937 rng(42); // fixed seed for reproducibility

    int passed = 0;
    int failed = 0;

    std::printf("Property Test 1: Camera3D struct validity after init\n");
    std::printf("Running %d iterations...\n", NUM_ITERATIONS);

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        // Generate random valid parameters
        Vector3 target{
            randomFloat(rng, -100.0f, 100.0f),
            randomFloat(rng, -100.0f, 100.0f),
            randomFloat(rng, -100.0f, 100.0f)
        };
        float distance = randomFloat(rng, 1.0f, 100.0f);
        float height = randomFloat(rng, -50.0f, 50.0f);
        float speed = randomFloat(rng, 0.01f, 10.0f);
        float fovy = randomFloat(rng, 10.0f, 120.0f);

        // Create and initialize camera
        chrono_clash::game::GameCamera cam;
        cam.init(target, distance, height, speed, fovy);

        // Get the Camera3D struct
        Camera3D c = cam.getCamera3D();

        bool iterPassed = true;

        // Assert: projection == CAMERA_PERSPECTIVE
        if (c.projection != CAMERA_PERSPECTIVE) {
            std::printf("  FAIL [iter %d]: projection=%d, expected=%d\n",
                        i, c.projection, CAMERA_PERSPECTIVE);
            iterPassed = false;
        }

        // Assert: up == (0, 1, 0)
        if (!approxEqual(c.up.x, 0.0f) || !approxEqual(c.up.y, 1.0f) || !approxEqual(c.up.z, 0.0f)) {
            std::printf("  FAIL [iter %d]: up=(%f, %f, %f), expected=(0, 1, 0)\n",
                        i, c.up.x, c.up.y, c.up.z);
            iterPassed = false;
        }

        // Assert: fovy == input fovy
        if (!approxEqual(c.fovy, fovy)) {
            std::printf("  FAIL [iter %d]: fovy=%f, expected=%f\n",
                        i, c.fovy, fovy);
            iterPassed = false;
        }

        // Assert: horizontal (xz-plane) distance from camera to target == distance (±0.01)
        float dx = c.position.x - target.x;
        float dz = c.position.z - target.z;
        float horizontalDist = std::sqrt(dx * dx + dz * dz);
        if (!approxEqual(horizontalDist, distance)) {
            std::printf("  FAIL [iter %d]: horizontal distance=%f, expected=%f (diff=%f)\n",
                        i, horizontalDist, distance, std::fabs(horizontalDist - distance));
            iterPassed = false;
        }

        // Assert: camera.position.y == target.y + height
        float expectedY = target.y + height;
        if (!approxEqual(c.position.y, expectedY)) {
            std::printf("  FAIL [iter %d]: position.y=%f, expected=%f (target.y=%f + height=%f)\n",
                        i, c.position.y, expectedY, target.y, height);
            iterPassed = false;
        }

        // Assert: camera target == configured target
        if (!approxEqual(c.target.x, target.x) ||
            !approxEqual(c.target.y, target.y) ||
            !approxEqual(c.target.z, target.z)) {
            std::printf("  FAIL [iter %d]: camera.target=(%f,%f,%f), expected=(%f,%f,%f)\n",
                        i, c.target.x, c.target.y, c.target.z,
                        target.x, target.y, target.z);
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
