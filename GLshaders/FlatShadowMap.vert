#version 410 core
layout (location = 0) in vec3 aPosition;

uniform mat4 directionalLightSpaceMatrix;
uniform mat4 modelMatrix;

void main()
{
    gl_Position = directionalLightSpaceMatrix * modelMatrix * vec4(aPosition, 1.0);
}
