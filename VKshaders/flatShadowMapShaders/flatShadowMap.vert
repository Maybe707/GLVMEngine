#version 450

// #extension GL_ARB_separate_shader_objects : enable
// #extension GL_ARB_shading_language_420pack : enable

#define MAX_JOINTS_NUMBER 128

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 lightSpace;
	mat4 jointMatrices[MAX_JOINTS_NUMBER];
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoordinate;
layout(location = 3) in vec4 inJointIndices;
layout(location = 4) in vec4 inWeights;

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
	
	// if (gl_VertexIndex % 2 == 0) {
	// 	gl_Position = vec4(0.5, 0.5, 0.5, 1.0);
	// 	} else {
	// 	gl_Position = vec4(0.7, 0.7, 0.7, 1.0);
	// 	}
    gl_Position = ubo.lightSpace * worldPosition;
//	gl_Position = vec4(inPosition, 1.0);
	// outFragmentPosition = vec3(ubo.model * vec4(inPosition, 1.0));
    // outFragmentNormal = inNormal;
    // outFragmentTextureCoordinate = inTextureCoordinate;
}


