#version 330 core
layout (location = 0) in vec3 aPosition;

uniform mat4 lightSpaceMatrix;
uniform mat4 aModel_Matrix;

void main()
{
    gl_Position = lightSpaceMatrix * aModel_Matrix * vec4(aPosition, 1.0);
}
