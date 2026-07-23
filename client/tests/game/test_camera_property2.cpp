/**
 * Property Test: Orbit position invariant
 *
 * Validates: Requirements 2.1, 2.2, 2.3, 2.6
 *
 * Property 2: For any GameCamera in Orbit mode, at any point during execution
 * (after any sequence of update(dt) calls with valid dt values), the camera
 * position SHALL satisfy:
 *   sqrt((cam.x - target.x)^2 + (cam.z - target.z)^2) == distance (±0.01)
 *   AND cam.y == target.y + height
 *   AND Camera3D.target == configured target
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

// Random int in [min, max]
int randomInt(std::mt19937& rng, int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
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

    std::printf("Property Test 2: Orbit position invariant\n");
    std::printf("Running %d iterations...\n", NUM_ITERATIONS);

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        // Generate random valid init parameters
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

        // Generate a random number of update steps (between 5 and 50)
        int numSteps = randomInt(rng, 5, 50);

        bool iterPassed = true;

        for (int step = 0; step < numSteps; ++step) {
            // Generate a random valid dt (positive, ≤ 0.1)
            float dt = randomFloat(rng, 0.001f, 0.1f);
            cam.update(dt);

            // Get the Camera3D struct after the update
            Camera3D c = cam.getCamera3D();

            // Assert: horizontal (xz-plane) distance from camera to target == distance (±0.01)
            float dx = c.position.x - target.x;
            float dz = c.position.z - target.z;
            float horizontalDist = std::sqrt(dx * dx + dz * dz);
            if (!approxEqual(horizontalDist, distance)) {
                std::printf("  FAIL [iter %d, step %d]: horizontal distance=%f, expected=%f (diff=%f)\n",
                            i, step, horizontalDist, distance, std::fabs(horizontalDist - distance));
                iterPassed = false;
                break;
            }

            // Assert: cam.position.y == target.y + height (±0.01)
            float expectedY = target.y + height;
            if (!approxEqual(c.position.y, expectedY)) {
                std::printf("  FAIL [iter %d, step %d]: position.y=%f, expected=%f (target.y=%f + height=%f)\n",
                            i, step, c.position.y, expectedY, target.y, height);
                iterPassed = false;
                break;
            }

            // Assert: Camera3D.target == configured target (±0.01 for each component)
            if (!approxEqual(c.target.x, target.x) ||
                !approxEqual(c.target.y, target.y) ||
                !approxEqual(c.target.z, target.z)) {
                std::printf("  FAIL [iter %d, step %d]: camera.target=(%f,%f,%f), expected=(%f,%f,%f)\n",
                            i, step, c.target.x, c.target.y, c.target.z,
                            target.x, target.y, target.z);
                iterPassed = false;
                break;
            }
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
