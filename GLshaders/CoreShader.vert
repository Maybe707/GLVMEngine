#version 410 core
// layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec2 aTexCoord;
// layout (location = 2) in vec3 aNormal;
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoordinates;
layout (location = 3) in vec4 jointIndices;
layout (location = 4) in vec4 weights;

#define SPOT_LIGHT_SPACE_MATRIX_CONTAINER_SIZE 8
#define DIRECTIONAL_LIGHT_SPACE_MATRIX_CONTAINER_SIZE 4

out vec2 textureCoords;
// flat out int spotLightSpaceMatrixArraySize;
// flat out int directionalLightSpaceMatrixArraySize;

out VS_OUT {
	vec3 fragmentPosition;
	vec3 normal;
	vec2 textureCoords;
	vec4 fragmentPositionDirectionalLightSpace[DIRECTIONAL_LIGHT_SPACE_MATRIX_CONTAINER_SIZE];
	vec4 fragmentPositionSpotLightSpace[SPOT_LIGHT_SPACE_MATRIX_CONTAINER_SIZE];
} vs_out;

//uniform mat4 rotateMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform mat4 directionalLightSpaceMatrixContainer[DIRECTIONAL_LIGHT_SPACE_MATRIX_CONTAINER_SIZE];
uniform int spotLightSpaceMatrixContainerSize;

uniform mat4 spotLightSpaceMatrixContainer[SPOT_LIGHT_SPACE_MATRIX_CONTAINER_SIZE];
uniform int directionalLightSpaceMatrixContainerSize;

uniform bool reverseNormals;

uniform mat4 jointMatrices[5];

//uniform mat4 inverseMatrices[32];
uniform mat4 inverseMatrix0;
uniform mat4 inverseMatrix1;
uniform mat4 inverseMatrix2;

uniform mat4 transform0;
uniform mat4 transform1;
uniform mat4 transform2;

void main()
{
    // gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition.x, vertexPosition.y, vertexPosition.z, 1.0);
	// fragmentPosition = vec3(modelMatrix * vec4(vertexPosition.x, vertexPosition.y, vertexPosition.z, 1.0));
 	// TextureCoord = vec2(aTextureCoord.x, aTextureCoord.y);	
	// normal = normal;
	vs_out.fragmentPosition           = vec3(modelMatrix * vec4(vertexPosition, 1.0));
//	vs_out.normal                     = transpose(inverse(mat3(modelMatrix))) * normal;
//	vs_out.normal                     = normal;
	if(reverseNormals) // a slight hack to make sure the outer large cube displays lighting from the 'inside' instead of the default 'outside'.
        vs_out.normal = transpose(inverse(mat3(modelMatrix))) * (-1.0 * normal);
    else
        vs_out.normal = transpose(inverse(mat3(modelMatrix))) * normal;
	vs_out.textureCoords              = textureCoordinates;
	for (int i = 0; i < directionalLightSpaceMatrixContainerSize; ++i) 
		vs_out.fragmentPositionDirectionalLightSpace[i] = directionalLightSpaceMatrixContainer[i] * vec4(vs_out.fragmentPosition, 1.0);
	for (int j = 0; j < spotLightSpaceMatrixContainerSize; ++j) 
		vs_out.fragmentPositionSpotLightSpace[j] = spotLightSpaceMatrixContainer[j] * vec4(vs_out.fragmentPosition, 1.0);

//	vs_out.normal = normalize(vs_out.normal);
	// spotLightSpaceMatrixArraySize        = spotLightSpaceMatrixContainerSize;
	// directionalLightSpaceMatrixArraySize = directionalLightSpaceMatrixContainerSize;

	mat4 skinMatrix =
		weights.x * jointMatrices[int(jointIndices.x)] +
		weights.y * jointMatrices[int(jointIndices.y)] +
		weights.z * jointMatrices[int(jointIndices.z)] +
		weights.w * jointMatrices[int(jointIndices.w)];

//	vec4 worldPosition = skinMatrix * inverseMatrices[int(jointIndices.x)] * vec4(vertexPosition, 1.0);
//	vec4 worldPosition = transform0 * transform1 * transform2 * vec4(vertexPosition, 1.0);
	vec4 worldPosition = skinMatrix * vec4(vertexPosition, 1.0);
//	vec4 worldPosition = transform0 * vec4(vertexPosition, 1.0);
	gl_Position   = projectionMatrix * viewMatrix * worldPosition;
}
