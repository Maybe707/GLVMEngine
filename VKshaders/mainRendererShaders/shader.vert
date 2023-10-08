#version 450

#define SPOT_LIGHT_SPACE_MATRIX_CONTAINER_SIZE 8
#define DIRECTIONAL_LIGHT_SPACE_MATRIX_CONTAINER_SIZE 4

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 3) out VS_OUT {
	vec3 fragmentPosition;
	vec3 normal;
	vec2 textureCoords;
	vec4 fragmentPositionDirectionalLightSpace[DIRECTIONAL_LIGHT_SPACE_MATRIX_CONTAINER_SIZE];
	vec4 fragmentPositionSpotLightSpace[SPOT_LIGHT_SPACE_MATRIX_CONTAINER_SIZE];
} vs_out;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoordinate;

layout(location = 0) out vec3 outFragmentPosition;
layout(location = 1) out vec3 outFragmentNormal;
layout(location = 2) out vec2 outFragmentTextureCoordinate;

layout(set = 1, binding = 1) uniform TestDirLightSpaceMatrixUBO {
	mat4 dirSpaceMatrix[DIRECTIONAL_LIGHT_SPACE_MATRIX_CONTAINER_SIZE];
} dirSpaceMat;

layout(set = 2, binding = 2) uniform SpotLightSpaceMatrixUBO {
	mat4 spotSpaceMatrix[SPOT_LIGHT_SPACE_MATRIX_CONTAINER_SIZE];
} spotSpaceMat;

void main() {
	vs_out.fragmentPosition = vec3(ubo.model * vec4(inPosition, 1.0));
	vs_out.normal = transpose(inverse(mat3(ubo.model))) * inNormal;
	vs_out.textureCoords = inTextureCoordinate;
	for (int i = 0; i < 1; ++i) 
		vs_out.fragmentPositionDirectionalLightSpace[i] = dirSpaceMat.dirSpaceMatrix[i] * vec4(vs_out.fragmentPosition, 1.0);
	
	for (int i = 0; i < 1; ++i) 
		vs_out.fragmentPositionSpotLightSpace[i] = spotSpaceMat.spotSpaceMatrix[i] * vec4(vs_out.fragmentPosition, 1.0);
	
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
	outFragmentPosition = vec3(ubo.model * vec4(inPosition, 1.0));
    outFragmentNormal = inNormal;
    outFragmentTextureCoordinate = inTextureCoordinate;
}
