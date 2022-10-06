#version 410 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragmentPosition;
in vec3 Normal;

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 lightPosition;

// Texture sampler
uniform sampler2D tex;

void main()
{
	float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

//    FragColor = vec4(result, 1.0);
	// vec3 resultColor = lightColor * objectColor;


	
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPosition - FragmentPosition);

	float difference = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = difference * lightColor;

	vec3 result = (ambient + diffuse) * objectColor;
    FragColor = texture(tex, TexCoord) * vec4(result, 1.0);
}
