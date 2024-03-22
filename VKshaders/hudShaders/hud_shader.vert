#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoordinate;
layout(location = 3) in vec4 inJointIndices;
layout(location = 4) in vec4 inWeights;

layout (set = 0, binding = 0) uniform HUD_UBO {
	vec3 entityPosition;
	bool isHudExists;
	int maxHP;
	int currentHP;
} hudUBO;

layout(location = 0) out VS_OUT {
	int value;
} vs_out;

void main()
{
	float scale = 0.1;
	
	mat4 skinMatrix = mat4(
		scale, 0.0, 0.0, 0.0,
		0.0, scale, 0.0, 0.0,
		0.0, 0.0, scale, 0.0,
		0.0, 0.0, 0.0, 1.0
		);

	
    gl_Position = skinMatrix * vec4(inPosition, 1.0);	
}
