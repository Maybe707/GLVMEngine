#version 410 core
out vec4 fragColor;

in VS_OUT {
	vec3 fragmentPosition;
	vec3 normal;
	vec2 textureCoords;
	vec4 fragmentPositionLightSpace;
} fs_in;

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

#define DIRECTIONAL_LIGHTS_NUMBER 32
#define POINT_LIGHTS_NUMBER       64
#define SPOT_LIGHTS_NUMBER        32

uniform bool             shadows;
uniform float            farPlane;
uniform vec3             lightPos;
uniform sampler2D        flatShadowMap;
uniform samplerCube      cubeShadowMap;
uniform sampler2D        diffuseTexture;
uniform int              directionalLightsArraySize;
uniform int              pointLightsArraySize;
uniform int              spotLightsArraySize;
uniform Material         material;
uniform DirectionalLight directionalLights[DIRECTIONAL_LIGHTS_NUMBER];
uniform PointLight       pointLights[POINT_LIGHTS_NUMBER];
uniform SpotLight        spotLights[SPOT_LIGHTS_NUMBER];
uniform vec3             viewPosition;

vec3 ComputeDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);
vec3 ComputePointLight(PointLight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection);
vec3 ComputeSpotLight(SpotLight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection);
float ComputeDirectionalShadow(vec4 fragmentPositionLightSpace);
float ComputePointShadow(vec3 fragmentPosition);

void main()
{
	vec3 normal        = normalize(fs_in.normal);
	vec3 viewDirection = normalize(viewPosition - fs_in.fragmentPosition);
	
	vec3 result = vec3(0.0, 0.0, 0.0);
	// Compute directional lighting	
	for (int f = 0; f < directionalLightsArraySize; ++f)
		result += ComputeDirectionalLight(directionalLights[f], normal, viewDirection);
	// Compute point lights
	for (int i = 0; i < pointLightsArraySize; ++i)
		result += ComputePointLight(pointLights[i], normal, fs_in.fragmentPosition, viewDirection);
	// Compute spot light
	for (int j = 0; j < spotLightsArraySize; ++j) 
		result += ComputeSpotLight(spotLights[j], normal, fs_in.fragmentPosition, viewDirection);

	// Compute shadow
	// float shadow = ComputeShadow(fs_in.fragmentPositionLightSpace);
	// result = result * (1.0 - shadow);
	
	fragColor = vec4(result, 1.0);

// 	vec3 color = texture(diffuseTexture, fs_in.textureCoords).rgb;
//     vec3 normal = normalize(fs_in.normal);
//     vec3 lightColor = vec3(0.3);
//     // ambient
//     vec3 ambient = 0.3 * lightColor;
//     // diffuse
//     vec3 lightDir = normalize(lightPos - fs_in.fragmentPosition);
//     float diff = max(dot(lightDir, normal), 0.0);
//     vec3 diffuse = diff * lightColor;
//     // specular
//     vec3 viewDir = normalize(viewPosition - fs_in.fragmentPosition);
//     vec3 reflectDir = reflect(-lightDir, normal);
//     float spec = 0.0;
//     vec3 halfwayDir = normalize(lightDir + viewDir);  
//     spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
//     vec3 specular = spec * lightColor;
//     // calculate shadow
// 	float shadow = 0.0;
//     shadow += ComputeDirectionalShadow(fs_in.fragmentPositionLightSpace);
// 	shadow += ComputePointShadow(fs_in.fragmentPosition);
//     vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
	
// //	fragColor = vec4(vec3(shadow / farPlane), 1.0);
// 	fragColor = vec4(lighting, 1.0);
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
	vec3 diffuse  = light.diffuse * difference * vec3(texture(material.diffuse, fs_in.textureCoords));
	vec3 specular = light.specular * specularComponent * vec3(texture(material.specular, fs_in.textureCoords));

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
	vec3 diffuse  = light.diffuse * difference * vec3(texture(material.diffuse, fs_in.textureCoords));
	vec3 specular = light.specular * specularComponent * vec3(texture(material.specular, fs_in.textureCoords));

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
	vec3 diffuse  = light.diffuse * difference * vec3(texture(material.diffuse, fs_in.textureCoords));
	vec3 specular = light.specular * specularComponent * vec3(texture(material.specular, fs_in.textureCoords));
//	ambient  *= attenuation * intensity;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;
	
    return (ambient + diffuse + specular);
}

float ComputeDirectionalShadow(vec4 fragmentPositionLightSpace) {
	// Perform perspective devide
	vec3 projectiveCoordinates = fragmentPositionLightSpace.xyz / fragmentPositionLightSpace.w;
	// Transform to [0.1] range
	projectiveCoordinates      = projectiveCoordinates * 0.5 + 0.5;
	// Get closest depth value from light's perspective (using [0,1] range fragmentPositionLight as coordinates)
	float closestDepth         = texture(flatShadowMap, projectiveCoordinates.xy).r;
	// Get depth of current fragment from light's perspective
	float currentDepth         = projectiveCoordinates.z;
	// Check whether current fragment position is in shadow
	vec3 normal = normalize(fs_in.normal);
//	vec3 lightDir = normalize(lightPos - fs_in.fragmentPositionLightSpace.xyz);
	vec3 lightDir = normalize(lightPos - fs_in.fragmentPosition);
	float bias                 = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
//	float shadow               = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(flatShadowMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(flatShadowMap, projectiveCoordinates.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 0.5 : 0.0;
		}
	}
	shadow /= 9.0;
	
	if (projectiveCoordinates.z > 1.0)
		shadow = 0.0;

	// if (projectiveCoordinates.x > 1.0 || projectiveCoordinates.x < -1.0)
	// 	shadow = 0.0;

	// if (projectiveCoordinates.y > 1.0 || projectiveCoordinates.y < -1.0)
	// 	shadow = 0.0;
		
	return shadow;
}

float ComputePointShadow(vec3 fragmentPosition) {
	// Get vector between fragment position and light position
	vec3 fragmentToLight       = fragmentPosition - lightPos;
	// Get the fragment to light vector to sample from the shadow map
	float closestDepth         = texture(cubeShadowMap, fragmentToLight).r;
	// It is currently in linear range between [0,1], let's re-transform it back to original depth value
	closestDepth              *= farPlane;
	// Now get current linear depth as the length between the fragment and light position
	float currentDepth         = length(fragmentToLight);
	// Test for shadows
	float bias                 = 0.05;
	float shadow               = currentDepth - bias > closestDepth ? 0.5 : 0.0;

//	return closestDepth;
	return shadow;
}
