#version 410 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragmentPosition;
in vec3 Normal;

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 ambient;
    // vec3 diffuse;
	sampler2D diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;
uniform Light light;
uniform vec3 viewPosition;

// Texture sampler
//uniform sampler2D tex;

void main()
{
    vec3 ambient = light.ambient * material.ambient;
//    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
	
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(light.position - FragmentPosition);
	float difference = max(dot(normal, lightDirection), 0.0f);
//	vec3 diffuse = light.diffuse * (difference * material.diffuse);
	vec3 diffuse = light.diffuse * difference * vec3(texture(material.diffuse, TexCoord));

	vec3 viewDirection = normalize(viewPosition - FragmentPosition);
	vec3 reflectDirection = reflect(-lightDirection, normal);
	float specularComponent = pow(max(dot(viewDirection, reflectDirection), 0.0f), material.shininess);
	vec3 specular = light.specular * (specularComponent * material.specular);
	
	vec3 result = ambient + diffuse + specular;
//	FragColor = texture(tex, TexCoord) * vec4(result, 1.0f);
//	FragColor = vec4 (0.4, 0.5, 0.4, 1.0) * vec4(0.5, 0.4, 0.4, 1.0);
	FragColor = vec4(result, 1.0f);
//	FragColor = texture(tex, TexCoord);
}
