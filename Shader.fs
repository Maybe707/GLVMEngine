#version 410 core
out vec4 FragColor;

in vec2 TexCoord;
// Texture sampler
uniform sampler2D tex;

void main()
{
	vec3 lightColor = vec3(0.33, 0.42, 0.18);
	vec3 objectColor = vec3(1.0, 0.5, 0.31);
	vec3 resultColor = lightColor * objectColor;
    FragColor = texture(tex, TexCoord) * vec4(resultColor, 1.0);
//    FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}
