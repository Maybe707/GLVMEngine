#include "Rendering/CameraOrbit.hpp"
#include <cassert>
#include <iostream>

using GLVM::core::rotateCameraOrbit;
using GLVM::ecs::components::beholder;
float elevation(const beholder& camera) {
    return std::atan2(camera.forward[1], std::hypot(camera.forward[0], camera.forward[2])) *
           180.0f / std::numbers::pi_v<float>;
}
void near(float actual, float expected, float tolerance = 0.002f) {
    assert(std::isfinite(actual) && std::abs(actual - expected) < tolerance);
}
void valid(beholder camera, float radius) {
    near(VecLength(camera.Position), radius);
    near(VecLength(camera.forward), 1.0f);
    const auto view = LookAtMain(camera.Position, camera.Position + camera.forward, vec3(0.0f, -1.0f, 0.0f));
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) assert(std::isfinite(view[i][j]));
}
int main() {
    beholder camera{.Position = {0.0f, 5.0f, 1.0f}}; // The demo starts tilted, not at zero pitch.
    const float radius = VecLength(camera.Position);
    rotateCameraOrbit(camera, 0, 0);
    const float initialElevation = elevation(camera);
    rotateCameraOrbit(camera, 0, -890);
    near(elevation(camera), initialElevation + 89.0f);
    // More than the old 890-pixel budget must still reach nearly vertical up.
    for (int i = 0; i < 20; ++i) rotateCameraOrbit(camera, 0, -100);
    near(elevation(camera), 89.0f);
    valid(camera, radius);
    rotateCameraOrbit(camera, 0, 1);
    near(elevation(camera), 88.9f); // No accumulated excess movement to unwind.
    rotateCameraOrbit(camera, 0, 10000);
    near(elevation(camera), -89.0f);
    rotateCameraOrbit(camera, 0, -1);
    near(elevation(camera), -88.9f);
    const vec3 beforeYaw = camera.Position;
    for (int i = 0; i < 36; ++i) rotateCameraOrbit(camera, 100, 0);
    near(elevation(camera), -88.9f);
    near(VecLength(camera.Position - beforeYaw), 0.0f);
    valid(camera, radius);
    const vec3 resting = camera.Position;
    for (int i = 0; i < 1000; ++i) rotateCameraOrbit(camera, 0, 0);
    near(VecLength(camera.Position - resting), 0.0f);
    beholder one{.Position = {0.0f, 0.0f, 5.0f}}, many = one;
    rotateCameraOrbit(one, 160, -80);
    for (int i = 0; i < 8; ++i) rotateCameraOrbit(many, 20, -10);
    near(VecLength(one.Position - many.Position), 0.0f);
    beholder empty;
    rotateCameraOrbit(empty, 10, -10);
    valid(empty, 1.0f);
    std::cout << "PASS camera full vertical range, immediate reversal, yaw at poles, frame partitioning and idle stability\n";
}
