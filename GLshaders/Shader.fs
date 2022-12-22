#version 410 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragmentPosition;
in vec3 Normal;

struct Material {
    vec3      ambient;
	sampler2D diffuse;
	sampler2D specular;
    float     shininess;
}; 

struct DirectionalLight {
	vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

	vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	vec3  position;
	vec3  direction;
	float cutOff;
	float outerCutOff;

	vec3  ambient;
	vec3  diffuse;
	vec3  specular;

	float constant;
	float linear;
	float quadratic;
};

#define POINT_LIGHTS_NUMBER 128
#define SPOT_LIGHTS_NUMBER  128

uniform int              directionalArraySize;
uniform int              pointLightsArraySize;
uniform int              spotLightsArraySize;
uniform Material         material;
uniform DirectionalLight directionalLight;
uniform PointLight       pointLights[POINT_LIGHTS_NUMBER];
uniform SpotLight        spotLights[SPOT_LIGHTS_NUMBER];
uniform vec3             viewPosition;

vec3 ComputeDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);
vec3 ComputePointLight(PointLight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection);
vec3 ComputeSpotLight(SpotLight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection);

void main()
{
	vec3 normal        = normalize(Normal);
	vec3 viewDirection = normalize(viewPosition - FragmentPosition);
	
//	vec3 lightDirection = normalize(spotLights[0].position - FragmentPosition);
	
//	vec3 result;
	// Compute directional lighting
	vec3 result;
	
	if (directionalArraySize > 0) {
		result += ComputeDirectionalLight(directionalLight, normal, viewDirection);
	}
	// Compute point lights
	for (int i = 0; i < pointLightsArraySize; ++i)
		result += ComputePointLight(pointLights[i], normal, FragmentPosition, viewDirection);
	// Compute spot light

	// spotlight intensity
// 	float theta     = dot(lightDirection, normalize(-spotLights[0].direction));
// 	float epsilon   = spotLights[0].cutOff - spotLights[0].outerCutOff;
// 	float intensity = clamp((theta - spotLights[0].outerCutOff) / epsilon, 0.0, 1.0);
// 	if (theta > spotLights[0].outerCutOff) {
// 		// diffuse shading
// 		float difference    = max(dot(normal, lightDirection), 0.0f);
// 		// specular shading
// 		vec3 reflectDirection   = reflect(-lightDirection, normal);
// 		float specularComponent = pow(max(dot(viewDirection, reflectDirection), 0.0f), material.shininess);
// 		// attenuation
// 		float distance    = length(spotLights[0].position - FragmentPosition);
// 		float attenuation = 1.0 / (spotLights[0].constant + spotLights[0].linear * distance + spotLights[0].quadratic * (distance * distance)); 

// 		// combine results
// 		vec3 ambient  = spotLights[0].ambient * material.ambient;
// 		vec3 diffuse  = spotLights[0].diffuse * difference * vec3(texture(material.diffuse, TexCoord));
// 		vec3 specular = spotLights[0].specular * specularComponent * vec3(texture(material.specular, TexCoord));
// //	ambient  *= attenuation * intensity;
// 		diffuse  *= attenuation * intensity;
// 		specular *= attenuation * intensity;

// 		result += ambient + diffuse + specular;
// 		FragColor = vec4(result, 1.0);
// 	}

	result += ComputeSpotLight(spotLights[0], normal, FragmentPosition, viewDirection); 
	
// 		vec3 ambient = light.ambient * material.ambient;
	
// 		vec3 normal = normalize(Normal);
// 		vec3 lightDirection = normalize(light.position - FragmentPosition);
// //		vec3 lightDirection = normalize(-light.direction);
// 		float difference = max(dot(normal, lightDirection), 0.0f);
// 		vec3 diffuse = light.diffuse * difference * vec3(texture(material.diffuse, TexCoord));

// 		vec3 viewDirection = normalize(viewPosition - FragmentPosition);
// 		vec3 reflectDirection = reflect(-lightDirection, normal);
// 		float specularComponent = pow(max(dot(viewDirection, reflectDirection), 0.0f), material.shininess);
// 		vec3 specular = light.specular * specularComponent * vec3(texture(material.specular, TexCoord));
	
//	FragColor = vec4(ambient + diffuse + specular, 1.0);

	FragColor = vec4(result, 1.0);
}

vec3 ComputeDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection) {
	vec3 lightDirection = normalize(-light.direction);
	// diffuse shading
	float difference    = max(dot(normal, lightDirection), 0.0f);
	// specular shading
	vec3 reflectDirection   = reflect(-lightDirection, normal);
	float specularComponent = pow(max(dot(viewDirection, reflectDirection), 0.0f), material.shininess);
	// combine results
	vec3 ambient  = light.ambient * material.ambient;
	vec3 diffuse  = light.diffuse * difference * vec3(texture(material.diffuse, TexCoord));
	vec3 specular = light.specular * specularComponent * vec3(texture(material.specular, TexCoord));

	return (ambient + diffuse + specular);
}

vec3 ComputePointLight(PointLight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection) {
	vec3 lightDirection = normalize(light.position - fragmentPosition);
	// diffuse shading
	float difference    = max(dot(normal, lightDirection), 0.0f);
	// specular shading
	vec3 reflectDirection   = reflect(-lightDirection, normal);
	float specularComponent = pow(max(dot(viewDirection, reflectDirection), 0.0f), material.shininess);
	// attenuation
	float distance    = length(light.position - fragmentPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// combine results
	vec3 ambient  = light.ambient * material.ambient;
	vec3 diffuse  = light.diffuse * difference * vec3(texture(material.diffuse, TexCoord));
	vec3 specular = light.specular * specularComponent * vec3(texture(material.specular, TexCoord));

	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 ComputeSpotLight(SpotLight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection) {
	vec3 lightDirection = normalize(light.position - fragmentPosition);
	// diffuse shading
	float difference    = max(dot(normal, lightDirection), 0.0f);
	// specular shading
	vec3 reflectDirection   = reflect(-lightDirection, normal);
	float specularComponent = pow(max(dot(viewDirection, reflectDirection), 0.0f), material.shininess);
	// attenuation
	float distance    = length(light.position - fragmentPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 
	// spotlight intensity
    float theta     = dot(lightDirection, normalize(-light.direction));
	float epsilon   = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	// combine results
	vec3 ambient  = light.ambient * material.ambient;
	vec3 diffuse  = light.diffuse * difference * vec3(texture(material.diffuse, TexCoord));
	vec3 specular = light.specular * specularComponent * vec3(texture(material.specular, TexCoord));
//	ambient  *= attenuation * intensity;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;
	
    return (ambient + diffuse + specular);
}
