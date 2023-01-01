#version 410 core
// layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec2 aTexCoord;
// layout (location = 2) in vec3 aNormal;
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoords;

out vec2 TextureCoords;

out VS_OUT {
	vec3 FragmentPosition;
	vec3 Normal;
	vec2 TextureCoords;
	vec4 FragmentPositionLightSpace;
} vs_out;

//uniform mat4 aRotate_Matrix;
uniform mat4 modelMatrix;
uniform mat4 aView_Matrix;
uniform mat4 aProjection_Matrix;
uniform mat4 lightSpaceMatrix;

void main()
{
    // gl_Position = aProjection_Matrix * aView_Matrix * modelMatrix * vec4(aPosition.x, aPosition.y, aPosition.z, 1.0);
	// FragmentPosition = vec3(modelMatrix * vec4(aPosition.x, aPosition.y, aPosition.z, 1.0));
 	// TextureCoord = vec2(aTextureCoord.x, aTextureCoord.y);	
	// Normal = aNormal;
	vs_out.FragmentPosition           = vec3(modelMatrix * vec4(aPosition, 1.0));
	vs_out.Normal                     = transpose(inverse(mat3(modelMatrix))) * aNormal;
//	vs_out.Normal                     = aNormal;
	vs_out.TextureCoords              = aTextureCoords;
	vs_out.FragmentPositionLightSpace = lightSpaceMatrix * vec4(vs_out.FragmentPosition, 1.0);
	gl_Position                       = aProjection_Matrix * aView_Matrix * modelMatrix * vec4(aPosition, 1.0);
}
