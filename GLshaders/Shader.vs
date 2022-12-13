#version 410 core
    
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 FragmentPosition;
out vec3 Normal;

//uniform mat4 aRotate_Matrix;
uniform mat4 aModel_Matrix;
uniform mat4 aView_Matrix;
uniform mat4 aProjection_Matrix;

void main()
{
    gl_Position = aProjection_Matrix * aView_Matrix * aModel_Matrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	FragmentPosition = vec3(aModel_Matrix * vec4(aPos.x, aPos.y, aPos.z, 1.0));
 	TexCoord = vec2(aTexCoord.x, aTexCoord.y);	
	Normal = aNormal;
}
