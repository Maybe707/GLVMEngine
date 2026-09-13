#pragma once
#include "Components/ViewComponent.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace GLVM::core {
// Relative mouse motion in screen pixels: positive X is right, positive Y is down.
// Clamp the camera angle, never the input, so reversing at a pole responds immediately.
inline void rotateCameraOrbit(ecs::components::beholder& camera, float deltaX, float deltaY) {
    constexpr float radiansPerPixel = std::numbers::pi_v<float> / 180.0f * 0.1f;
    constexpr float pitchLimit = std::numbers::pi_v<float> / 180.0f * 89.0f;
    float radius = VecLength(camera.Position);
    if (radius < 0.001f) {
        camera.Position = {0.0f, 0.0f, 1.0f};
        radius = 1.0f;
    }
    if (deltaX != 0.0f || deltaY != 0.0f) {
        const float horizontal = std::hypot(camera.Position[0], camera.Position[2]);
        const float pitch = std::clamp(std::atan2(camera.Position[1], horizontal) +
                                       deltaY * radiansPerPixel, -pitchLimit, pitchLimit);
        const float yaw = std::remainder(std::atan2(camera.Position[0], camera.Position[2]) +
                                         deltaX * radiansPerPixel, 2.0f * std::numbers::pi_v<float>);
        camera.Position = vec3(std::sin(yaw) * std::cos(pitch), std::sin(pitch),
                               std::cos(yaw) * std::cos(pitch)) * radius;
    }
    camera.forward = Normalize(-camera.Position);
}
}
