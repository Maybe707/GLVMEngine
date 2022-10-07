#version 410 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragmentPosition;
in vec3 Normal;

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

// Texture sampler
uniform sampler2D tex;

void main()
{
	float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPosition - FragmentPosition);
	float difference = max(dot(normal, lightDirection), 0.0f);
    vec3 diffuse = difference * lightColor;

	float specularStrength = 0.5f;
	vec3 viewDirection = normalize(viewPosition - FragmentPosition);
	vec3 reflectDirection = reflect(-lightDirection, normal);
	float specularComponent = pow(max(dot(viewDirection, reflectDirection), 0.0f), 64);
	vec3 specular = specularStrength * specularComponent * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
//    FragColor = texture(tex, TexCoord) * vec4(result, 1.0f);
	FragColor = vec4(result, 1.0f);
}
