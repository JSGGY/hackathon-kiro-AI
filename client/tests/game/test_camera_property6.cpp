/**
 * Property Test: Delta-time clamping
 *
 * Validates: Requirements 3.4
 *
 * Property 6: For any dt value greater than 0.1 seconds, calling update(dt) SHALL
 * produce the same state change as calling update(0.1), ensuring large frame spikes
 * do not cause discontinuous jumps.
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

    std::printf("Property Test 6: Delta-time clamping\n");
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

        // Generate a random dt > 0.1 (range [0.101, 10.0])
        float largeDt = randomFloat(rng, 0.101f, 10.0f);

        // Camera A: update with large dt (should be clamped to 0.1)
        chrono_clash::game::GameCamera camA;
        camA.init(target, distance, height, speed, fovy);
        camA.update(largeDt);

        // Camera B: update with exactly 0.1
        chrono_clash::game::GameCamera camB;
        camB.init(target, distance, height, speed, fovy);
        camB.update(0.1f);

        bool iterPassed = true;

        // Compare angles
        float angleA = camA.getAngle();
        float angleB = camB.getAngle();
        if (!approxEqual(angleA, angleB)) {
            std::printf("  FAIL [iter %d]: Angle mismatch: A=%f, B=%f (diff=%f)\n",
                        i, angleA, angleB, std::fabs(angleA - angleB));
            std::printf("    largeDt=%f, speed=%f\n", largeDt, speed);
            iterPassed = false;
        }

        // Compare Camera3D structs
        Camera3D c3dA = camA.getCamera3D();
        Camera3D c3dB = camB.getCamera3D();
        if (!camera3DEqual(c3dA, c3dB)) {
            std::printf("  FAIL [iter %d]: Camera3D mismatch between large dt and clamped dt\n", i);
            std::printf("    largeDt=%f, speed=%f\n", largeDt, speed);
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
