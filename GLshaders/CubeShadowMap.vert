#version 410 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoordinates;
layout (location = 3) in vec4 jointIndices;
layout (location = 4) in vec4 weights;

uniform mat4 modelMatrix;

void main()
{
	gl_Position = modelMatrix * vec4(aPosition, 1.0);
}
