#version 330 core
    
layout (location = 0) in vec3 aPos;
    
void main()
{
    mat4 m;
    m[0] = vec4(1, 0, 0, 0);
    m[1] = vec4(0, 1, 0, 0);
    m[2] = vec4(0, 0, 1, 0);
    m[3] = vec4(0.2, 0.2, 0.2, 1);
    gl_Position = m * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}