#version 410

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoordinate;
layout(location = 3) in vec4 inJointIndices;
layout(location = 4) in vec4 inWeights;

layout (std140) uniform HUD_UBO {
	mat4 view;
	mat4 proj;
	vec3 entityPosition;
	int isHudExists;
	float maxHP;
	float currentHP;
	float highestY;
} hudUBO;

out VS_OUT {
	vec4 vertexPosition;
	vec4 entityPosition;
	float maxHP;
	float currentHP;
	float highestY;
} vs_out;

void main()
{
 	float scale = 0.2;

	mat4 modelMatrix = mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		hudUBO.entityPosition.x - 2.0,
		hudUBO.entityPosition.y,
		hudUBO.entityPosition.z - 0.5,
		1.0
		);
	
	mat4 scaleMatrix = mat4(
		scale, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, scale, 0.0,
		0.0, 0.0, 0.0, 1.0
		);

	vs_out.vertexPosition = modelMatrix * scaleMatrix * vec4(inPosition, 1.0);
	vs_out.entityPosition = vec4(hudUBO.entityPosition, 1.0);
	vs_out.highestY  = hudUBO.entityPosition.y;
	vs_out.currentHP = hudUBO.currentHP;
	vs_out.maxHP = hudUBO.maxHP;
    gl_Position = hudUBO.proj * hudUBO.view * modelMatrix * scaleMatrix * vec4(inPosition, 1.0);
}
