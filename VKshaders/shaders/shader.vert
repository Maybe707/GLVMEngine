#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(binding = 3) uniform UniformBufferObject2 {
	float value;
} ubo2;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoordinate;

layout(location = 0) out vec3 outFragmentPosition;
layout(location = 1) out vec3 outFragmentNormal;
layout(location = 2) out vec2 outFragmentTextureCoordinate;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    outFragmentNormal = inNormal;
    outFragmentTextureCoordinate = inTextureCoordinate;
}
