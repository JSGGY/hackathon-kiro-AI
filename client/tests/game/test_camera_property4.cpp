/**
 * Property Test: Update additivity (framerate independence)
 *
 * Validates: Requirements 3.2
 *
 * Property 4: For any sequence of positive delta-time values dt1, dt2 each <= 0.1,
 * calling update(dt1) then update(dt2) SHALL produce the same camera state as a
 * single call update(dt1 + dt2) (when the sum also <= 0.1), demonstrating that
 * camera movement is purely a function of accumulated time.
 */

#include "camera.hpp"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <random>

namespace {

constexpr int NUM_ITERATIONS = 200;
constexpr float TOLERANCE = 0.001f;

// Random float in [min, max]
float randomFloat(std::mt19937& rng, float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

bool approxEqual(float a, float b, float tol = TOLERANCE) {
    return std::fabs(a - b) <= tol;
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

    std::printf("Property Test 4: Update additivity (framerate independence)\n");
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

        // Generate 2 random positive dt values where each <= 0.1 and sum <= 0.1
        float dt1 = randomFloat(rng, 0.001f, 0.05f);
        float dt2 = randomFloat(rng, 0.001f, 0.1f - dt1);

        float dtCombined = dt1 + dt2;

        // Camera A: call update(dt1) then update(dt2)
        chrono_clash::game::GameCamera camA;
        camA.init(target, distance, height, speed, fovy);
        camA.update(dt1);
        camA.update(dt2);

        // Camera B: call update(dt1 + dt2)
        chrono_clash::game::GameCamera camB;
        camB.init(target, distance, height, speed, fovy);
        camB.update(dtCombined);

        bool iterPassed = true;

        // Compare angles
        float angleA = camA.getAngle();
        float angleB = camB.getAngle();
        if (!approxEqual(angleA, angleB)) {
            std::printf("  FAIL [iter %d]: Angle mismatch: A=%f, B=%f (diff=%f)\n",
                        i, angleA, angleB, std::fabs(angleA - angleB));
            std::printf("    dt1=%f, dt2=%f, combined=%f, speed=%f\n",
                        dt1, dt2, dtCombined, speed);
            iterPassed = false;
        }

        // Compare Camera3D structs
        Camera3D c3dA = camA.getCamera3D();
        Camera3D c3dB = camB.getCamera3D();
        if (!camera3DEqual(c3dA, c3dB)) {
            std::printf("  FAIL [iter %d]: Camera3D mismatch between sequential and combined update\n", i);
            std::printf("    dt1=%f, dt2=%f, combined=%f\n", dt1, dt2, dtCombined);
            std::printf("    A: pos=(%f,%f,%f) target=(%f,%f,%f) fovy=%f\n",
                        c3dA.position.x, c3dA.position.y, c3dA.position.z,
                        c3dA.target.x, c3dA.target.y, c3dA.target.z, c3dA.fovy);
            std::printf("    B: pos=(%f,%f,%f) target=(%f,%f,%f) fovy=%f\n",
                        c3dB.position.x, c3dB.position.y, c3dB.position.z,
                        c3dB.target.x, c3dB.target.y, c3dB.target.z, c3dB.fovy);
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
