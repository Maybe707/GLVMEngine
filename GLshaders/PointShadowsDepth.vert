#version 410 core
layout (location = 0) in vec3 aPosition;

uniform mat4 aModel_Matrix;

void main()
{
	gl_Position = aModel_Matrix * vec4(aPosition, 1.0);
}
