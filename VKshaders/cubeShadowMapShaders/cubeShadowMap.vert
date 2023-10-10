#version 450

// #extension GL_ARB_separate_shader_objects : enable
// #extension GL_ARB_shading_language_420pack : enable

#define CUBE_DEMENTIONS 6

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
	mat4 spaceMatrix;
	vec3 lightPosition;
	float farPlane;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoordinate;

layout(location = 0) out vec4 outFragmentPosition;
layout(location = 1) out vec3 outLightPosition;
layout(location = 2) out float outFarPlane;

void main() {
	// if (gl_VertexIndex % 2 == 0) {
	// 	gl_Position = vec4(0.5, 0.5, 0.5, 1.0);
	// 	} else {
	// 	gl_Position = vec4(0.7, 0.7, 0.7, 1.0);
	// 	}
    outFragmentPosition = ubo.spaceMatrix * ubo.model * vec4(inPosition, 1.0);
	// outFragmentPosition = vec3(ubo.model * vec4(inPosition, 1.0));
	// vec3 FragmentPosition = vec3(ubo.spaceMatrix * vec4(outFragmentPosition, 1.0));
	// outLightPosition = ubo.lightPosition;
	// outFarPlane = ubo.farPlane;
	// float lightDistance = length(fragmentPosition - ubo.lightPosition);
	// lightDistance = lightDistance / ubo.farPlane;

	// float lightDistance = length(outFragmentPosition - vec4(ubo.lightPosition, 1.0));

	
	// float lightDistance = length(outFragmentPosition - vec4(ubo.lightPosition, 1.0));

	// lightDistance = lightDistance / ubo.farPlane;

	
	// lightDistance = lightDistance / inFarPlane;
	
//	gl_Position.z = fragmentPosition.z;
	gl_Position = outFragmentPosition;
	// outFragmentPosition = vec3(ubo.model * vec4(inPosition, 1.0));
    // outFragmentNormal = inNormal;
    // outFragmentTextureCoordinate = inTextureCoordinate;
}


