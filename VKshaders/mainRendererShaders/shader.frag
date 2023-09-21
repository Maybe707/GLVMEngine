#version 450

// layout(set = 1, binding = 1) uniform sampler2D texSampler;

// layout(location = 0) in vec3 fragColor;
// layout(location = 1) in vec3 fragmentNormal;
// layout(location = 2) in vec2 fragTexCoord;

// layout(location = 0) out vec4 outColor;

// void main() {
//     outColor = texture(texSampler, fragTexCoord);
// }

#define DIRECTIONAL_LIGHTS_NUMBER                          4
#define POINT_LIGHTS_NUMBER                                32
#define SPOT_LIGHTS_NUMBER                                 8

layout(location = 0) in vec3 inFragmentPosition;
layout(location = 1) in vec3 inFragmentNormal;
layout(location = 2) in vec2 inFragmentTextureCoordinate;

layout(location = 3) in VS_OUT {
	vec3 fragmentPosition;
	vec3 normal;
	vec2 textureCoords;
	vec4 fragmentPositionDirectionalLightSpace[DIRECTIONAL_LIGHTS_NUMBER];
	vec4 fragmentPositionSpotLightSpace[SPOT_LIGHTS_NUMBER];
} fs_in;

layout(location = 0) out vec4 outColor;

layout(set = 3, binding = 3) uniform ViewPositionUBO {
	vec3 viewPosition;
} viewPos;

layout(set = 4, binding = 4) uniform MaterialUBO {
    vec3      ambient;
    float     shininess;
} material; 

struct DirectionalLight {
	vec3 position;
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

#define DIRECTIONAL_LIGHTS_NUMBER                          4
#define POINT_LIGHTS_NUMBER                                32
#define SPOT_LIGHTS_NUMBER                                 8

layout(set = 5, binding = 5) uniform DirectionalLightsUBO {
	DirectionalLight directionalLightsArray[DIRECTIONAL_LIGHTS_NUMBER];
	int directionalLightsArraySize;
} directionalLights;

layout(set = 6, binding = 6) uniform PointLightsUBO {
	PointLight pointLightsArray[POINT_LIGHTS_NUMBER];
	int pointLightsArraySize;
	float farPlane;
} pointLights;

layout(set = 7, binding = 7) uniform SpotLightsUBO {
	SpotLight spotLightsArray[SPOT_LIGHTS_NUMBER];
	int spotLightArraySize;
} spotLights;

layout(set = 8, binding = 8) uniform sampler2D diffuse;
layout(set = 9, binding = 9) uniform sampler2D specular;
layout(set = 10, binding = 10) uniform sampler2D shadowMap;
layout(set = 11, binding = 11) uniform samplerCube cubeShadowMap;

vec3 ComputeDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);
vec3 ComputePointLight(PointLight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection);
vec3 ComputeSpotLight(SpotLight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection);

float ComputeDirectionalShadow(DirectionalLight light, vec4 fragmentPositionDirectionalLightSpace, sampler2D flatShadowMap);
float ComputePointShadow(PointLight light, vec3 fragmentPosition, samplerCube cubeShadowMap);
float ComputeSpotShadow(SpotLight light, vec4 fragmentPositionSpotLightSpace, sampler2D flatShadowMap);

void main()
{
	vec3 fragmentNormal = normalize(fs_in.normal);
	vec3 viewDirection  = normalize(viewPos.viewPosition - fs_in.fragmentPosition);

	vec3 result = vec3(0.0, 0.0, 0.0);
	// for(int i = 0; i < directionalLights.directionalLightsArraySize; ++i ) {
	// 	vec3 light = ComputeDirectionalLight(directionalLights.directionalLightsArray[i], fragmentNormal, viewDirection);
	// 	float shadow = ComputeDirectionalShadow(directionalLights.directionalLightsArray[i], fs_in.fragmentPositionDirectionalLightSpace[0], shadowMap);
	// 	result += (1.0 - shadow) * light;
	// 	if(shadow > 0.0)
	// 		shadow = 0.0;
	// }

	for(int i = 0; i < pointLights.pointLightsArraySize; ++i) {
		vec3 light = ComputePointLight(pointLights.pointLightsArray[i], fragmentNormal, inFragmentPosition, viewDirection);
		float shadow = ComputePointShadow(pointLights.pointLightsArray[0],
										  inFragmentPosition, cubeShadowMap);
		result += (1.0 - shadow) * light;
		if(shadow > 0.0)
			shadow = 0.0;
	}

	// for(int i = 0; i < spotLights.spotLightArraySize; ++i) {
	// 	vec3 light = ComputeSpotLight(spotLights.spotLightsArray[i], fragmentNormal,
	// 								  inFragmentPosition, viewDirection);
	// 	float shadow = ComputeSpotShadow(spotLights.spotLightsArray[i],
	// 									 fs_in.fragmentPositionSpotLightSpace[0], shadowMap);

	// 	result += (1.0 - shadow) * light;
	// 	if(shadow > 0.0)
	// 		shadow = 0.0;
	// }

//    float depthValue = texture(shadowMap, inFragmentTextureCoordinate).r;
//x	float depthValue = texture(cubeShadowMap, vec3(inFragmentTextureCoordinate, 0)).r;
	
	outColor = vec4(result, 1.0);
//	outColor = vec4(vec3(depthValue), 1.0);
	
//	vec3 result = material.ambient * pointLights.pointLightsArray[0].diffuse;

	// vec3 lightDirection = normalize(pointLights.pointLightsArray[0].position - inFragmentPosition);
	// float differece = max(dot(fragmentNormal, lightDirection), 0.0);

	// vec3 diffuse = differece * pointLights.pointLightsArray[0].ambient;
	
	// outColor = vec4(diffuse, 1.0);
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
	vec3 diffuse  = light.diffuse * difference * vec3(texture(diffuse, fs_in.textureCoords));
	vec3 specular = light.specular * specularComponent * vec3(texture(specular, fs_in.textureCoords));

	return (ambient + diffuse + specular);
}

vec3 ComputePointLight(PointLight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection) {
	vec3 lightDirection = normalize(light.position - fragmentPosition);
	// Diffuse shading
	float difference    = max(dot(normal, lightDirection), 0.0f);
	// specular shading
	vec3 reflectDirection   = reflect(lightDirection, normal);
	float specularComponent = pow(max(dot(viewDirection, reflectDirection), 0.0f), material.shininess);
	// attenuation
	float distance    = length(light.position - fragmentPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// combine results
	vec3 ambient  = light.ambient * material.ambient;
	vec3 diffuse  = light.diffuse * difference * vec3(texture(diffuse, inFragmentTextureCoordinate));
	vec3 specular = light.specular * specularComponent * vec3(texture(specular, inFragmentTextureCoordinate));

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
	vec3 diffuse  = light.diffuse * difference * vec3(texture(diffuse, inFragmentTextureCoordinate));
	vec3 specular = light.specular * specularComponent * vec3(texture(specular, inFragmentTextureCoordinate));
	ambient  *= attenuation * intensity;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;
	
    return vec3(ambient + diffuse + specular);
}

float ComputeDirectionalShadow(DirectionalLight light, vec4 fragmentPositionDirectionalLightSpace, sampler2D flatShadowMap) {
	// Perform perspective devide
	vec3 projectiveCoordinates = fragmentPositionDirectionalLightSpace.xyz / fragmentPositionDirectionalLightSpace.w;
	// Transform to [0.1] range
	vec3 projectiveCoordinatesZO      = projectiveCoordinates * 0.5 + 0.5;
	// Get closest depth value from light's perspective (using [0,1] range fragmentPositionLight as coordinates)
//	float closestDepth         = texture(flatShadowMap, projectiveCoordinatesZO.xy).r;
	// Get depth of current fragment from light's perspective
	float currentDepth         = projectiveCoordinates.z;
	// Check whether current fragment position is in shadow
	vec3 normal = normalize(fs_in.normal);
//	vec3 lightDir = normalize(lightPos - fs_in.fragmentPositionPointLightSpace.xyz);
	vec3 lightDir = normalize(light.position - vec3(fs_in.fragmentPosition));
//	vec3 lightDir = normalize(vec3(fs_in.fragmentPosition) - light.position);
	float bias                 = max(0.01 * (1.0 - dot(normal, lightDir)), 0.005);
//	float shadow               = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	
	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(flatShadowMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(flatShadowMap, projectiveCoordinatesZO.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;
	
	if (projectiveCoordinatesZO.z > 1.0)
		shadow = 0.0;

	// if (projectiveCoordinates.x > 1.0 || projectiveCoordinates.x < -1.0)
	// 	shadow = 0.0;

	// if (projectiveCoordinates.y > 1.0 || projectiveCoordinates.y < -1.0)
	// 	shadow = 0.0;
		
	return shadow;
}

float ComputePointShadow(PointLight light, vec3 fragmentPosition, samplerCube cubeShadowMap) {
	// Get vector between fragment position and light position
	vec3 fragmentToLight       = fragmentPosition - light.position;
	// // Get the fragment to light vector to sample from the shadow map
	// float closestDepth         = texture(cubeShadowMap, fragmentToLight).r;
	// // It is currently in linear range between [0,1], let's re-transform it back to original depth value
	// closestDepth              *= farPlane;
	// Now get current linear depth as the length between the fragment and light position
	float currentDepth         = length(fragmentToLight);
	// Test for shadows simple
	// float bias                 = 0.05;
	// float shadow               = currentDepth - bias > closestDepth ? 0.5 : 0.0;

	// Test for shadows full solid cube of offsets
	// float bias    = 0.05;
	// float shadow  = 0.0;
	// float samples = 4.0;
	// float offset  = 0.1;
	// for(float x = -offset; x < offset; x += offset / (samples * 0.5))
	// 	{
	// 		for(float y = -offset; y < offset; y += offset / (samples * 0.5))
	// 			{
	// 				for(float z = -offset; z < offset; z += offset / (samples * 0.5))
	// 					{
	// 						float closestDepth = texture(cubeShadowMap, fragmentToLight + vec3(x, y, z)).r;
	// 						closestDepth *= farPlane; ///< undo mapping [0;1]
	// 						if(currentDepth - bias > closestDepth)
	// 							shadow += 1.0;
	// 					}
	// 			}
	// 	}
	// shadow /= (samples * samples * samples);

	// Test for shadows ranged cube of offsets
	vec3 sampleOffsetDirections[20] = vec3[]
		(
			vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
			vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
			vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
			vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
			vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
		);  
	
	float shadow  = 0.0;
	float bias    = 0.15;
	float samples = 20;
	float viewDistance = length(viewPos.viewPosition - fragmentPosition);
	float diskRadius   = (1.0 + (viewDistance / pointLights.farPlane)) / 25.0;
	for(int i = 0; i < samples; ++i)
		{
			float closestDepth = texture(cubeShadowMap, fragmentToLight + sampleOffsetDirections[i] *
										 diskRadius).r;
			closestDepth *= pointLights.farPlane; // undo mapping [0;1]
			if(currentDepth - bias > closestDepth)
				shadow += 1.0;
		}
	shadow /= float(samples);

//	return closestDepth;
	return shadow;
}

float ComputeSpotShadow(SpotLight light, vec4 fragmentPositionSpotLightSpace, sampler2D flatShadowMap) {
	// Perform perspective devide
	vec3 projectiveCoordinates = fragmentPositionSpotLightSpace.xyz / fragmentPositionSpotLightSpace.w;
	// Transform to [0.1] range
	vec3 projectiveCoordinatesZO      = projectiveCoordinates * 0.5 + 0.5;
	// Get closest depth value from light's perspective (using [0,1] range fragmentPositionLight as coordinates)
//	float closestDepth         = texture(flatShadowMap, projectiveCoordinatesZO.xy).r;
	// Get depth of current fragment from light's perspective
	float currentDepth         = projectiveCoordinates.z;
	// Check whether current fragment position is in shadow
	vec3 normal = normalize(fs_in.normal);
//	vec3 lightDir = normalize(lightPos - fs_in.fragmentPositionPointLightSpace.xyz);
	vec3 lightDir = normalize(light.position - fs_in.fragmentPosition);
	float bias                 = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
//	float shadow               = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(flatShadowMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(flatShadowMap, projectiveCoordinatesZO.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;
	
	if (projectiveCoordinatesZO.z > 1.0)
		shadow = 0.0;

	// if (projectiveCoordinates.x > 1.0 || projectiveCoordinates.x < -1.0)
	// 	shadow = 0.0;

	// if (projectiveCoordinates.y > 1.0 || projectiveCoordinates.y < -1.0)
	// 	shadow = 0.0;
		
	return shadow;
}
