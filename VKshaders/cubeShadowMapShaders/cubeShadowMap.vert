#version 450

// #extension GL_ARB_separate_shader_objects : enable
// #extension GL_ARB_shading_language_420pack : enable

#define MAX_JOINTS_NUMBER 128
#define CUBE_DEMENTIONS 6

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
	mat4 spaceMatrix;
	vec3 lightPosition;
	float farPlane;
	mat4 jointMatrices[MAX_JOINTS_NUMBER];
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoordinate;
layout(location = 3) in vec4 inJointIndices;
layout(location = 4) in vec4 inWeights;

layout(location = 0) out vec4 outFragmentPosition;
layout(location = 1) out vec3 outLightPosition;
layout(location = 2) out float outFarPlane;

void main() {
    mat4 skinMatrix = mat4(0.0);
    float totalWeight = 0.0;
    for (int influence = 0; influence < 4; ++influence) {
        int joint = int(inJointIndices[influence]);
        float weight = inWeights[influence];
        if (joint >= 0 && joint < MAX_JOINTS_NUMBER && weight > 0.0) {
            skinMatrix += weight * ubo.jointMatrices[joint];
            totalWeight += weight;
        }
    }
    if (totalWeight == 0.0) skinMatrix = mat4(1.0);


	vec4 worldPosition = ubo.model * skinMatrix * vec4(inPosition, 1.0);
    outFragmentPosition = worldPosition;
//	outFragmentPosition = ubo.spaceMatrix * worldPosition;
	outLightPosition = ubo.lightPosition;
	outFarPlane = ubo.farPlane;

	gl_Position = ubo.spaceMatrix * worldPosition;
}


