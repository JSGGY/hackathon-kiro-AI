/**
 * Property Test: Angle wrapping invariant
 *
 * Validates: Requirements 2.5
 *
 * Property 5: For any sequence of update calls that cause the cumulative angle
 * to exceed 2π, the stored orbital angle SHALL always remain in the range [0, 2π),
 * preventing unbounded growth.
 */

#include "camera.hpp"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <random>

namespace {

constexpr int NUM_ITERATIONS = 200;
constexpr float TWO_PI = 2.0f * 3.14159265358979323846f;

// Random float in [min, max]
float randomFloat(std::mt19937& rng, float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

} // anonymous namespace

int main() {
    std::mt19937 rng(42); // fixed seed for reproducibility

    int passed = 0;
    int failed = 0;

    std::printf("Property Test 5: Angle wrapping invariant\n");
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

        chrono_clash::game::GameCamera cam;
        cam.init(target, distance, height, speed, fovy);

        // Calculate how many dt steps we need so that speed * total_dt > 2π
        // Each dt is at most 0.1, so we need at least ceil(2π / (speed * 0.1)) steps
        // Add extra steps to ensure wrapping happens multiple times
        float totalTimeNeeded = (TWO_PI / speed) * 2.0f; // 2 full rotations worth
        int numSteps = static_cast<int>(totalTimeNeeded / 0.05f) + 10; // use dt ~0.05 average

        // Cap steps to prevent extremely long iterations for very slow speeds
        if (numSteps > 5000) numSteps = 5000;

        bool iterPassed = true;

        for (int step = 0; step < numSteps; ++step) {
            float dt = randomFloat(rng, 0.001f, 0.1f);
            cam.update(dt);

            float angle = cam.getAngle();

            // Assert angle is in [0, 2π)
            if (angle < 0.0f || angle >= TWO_PI) {
                std::printf("  FAIL [iter %d, step %d]: angle=%f out of range [0, 2π)\n",
                            i, step, angle);
                std::printf("    speed=%f, dt=%f, TWO_PI=%f\n", speed, dt, TWO_PI);
                iterPassed = false;
                break; // one failure is enough for this iteration
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
