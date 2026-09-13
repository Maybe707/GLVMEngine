#pragma once
#include "ShaderStructs.hpp"
#include "WavefrontObjParser.hpp"
#include <cmath>
#include <cfloat>
#ifndef MAXFLOAT
#define MAXFLOAT FLT_MAX
#endif
namespace GLVM::core {
	struct MeshAxisMaxAbsoluteValues {
		float absolute_x = 0.0f;
		float absolute_y = 0.0f;
		float absolute_z = 0.0f;

		float origin_offset_x = 0.0f;
		float origin_offset_y = 0.0f;
		float origin_offset_z = 0.0f;
	};

	struct MeshAxisLimitingValues {
		float lowest_x = MAXFLOAT;
		float highest_x = -MAXFLOAT;
		float lowest_y = MAXFLOAT;
		float highest_y = -MAXFLOAT;
		float lowest_z = MAXFLOAT;
		float highest_z = -MAXFLOAT;

		void setToDefaultValues() {
			highest_x = -MAXFLOAT;
			lowest_x  = MAXFLOAT;
			highest_y = -MAXFLOAT;
			lowest_y  = MAXFLOAT;
			highest_z = -MAXFLOAT;
			lowest_z  = MAXFLOAT;
		}

		void comparePerDirectionAndSetToMaximumValueByModule( SVertex& vertex ) {
			if ( vertex[0] < lowest_x ) {
				lowest_x = vertex[0];
			} else if ( vertex[0] > highest_x ) {
				highest_x = vertex[0];
			}

			if ( vertex[1] < lowest_y ) {
				lowest_y = vertex[1];
			} else if ( vertex[1] > highest_y ) {
				highest_y = vertex[1];
			}

			if ( vertex[2] < lowest_z ) {
				lowest_z = vertex[2];
			} else if ( vertex[2] > highest_z ) {
				highest_z = vertex[2];
			}
		}

		void comparePerDirectionAndSetToMaximumValueByModule( vec3 position, float half_x, float half_y, float half_z ) {
			if ( position[0] + half_x > highest_x ) {
				highest_x = position[0] + half_x;
			}
			if ( position[0] - half_x < lowest_x ) {
				lowest_x = position[0] - half_x;
			}
			if ( position[1] + half_y > highest_y ) {
				highest_y = position[1] + half_y;
			}
			if ( position[1] - half_y < lowest_y ) {
				lowest_y = position[1] - half_y;
			}
			if ( position[2] + half_z > highest_z ) {
				highest_z = position[2] + half_z;
			}
			if ( position[2] - half_z < lowest_z ) {
				lowest_z = position[2] - half_z;
			}
		}
	};

	struct Vertex {
        vec3 pos;
        vec3 color;
        vec2 texCoord;
		vec4 joinIndices;
		vec4 weights;

    };
}
/// Render objects
struct RenderPlayer {
	vec3 position;
	vec3 forward;
};

struct RenderActor {
	mat4 modelMatrix;
	GLVM::core::vector<mat4> jointMatrices;
	unsigned int meshID;
	unsigned int diffuseTextureIndex;
	unsigned int specularTextureIndex;
	vec3 ambient;
	float shininess;
};

struct RenderDirectionalLight {
	mat4 DirectionalLightSpaceMatrix;
	vec4 position;
	vec4 direction;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

struct RenderSpotLight {
	mat4 SpotLigthSpaceMatrix;
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

#define CUBE_MAP_LAYER_NUMBER 6

struct RenderPointLight {
	mat4 pointLightSpaceMatrix[CUBE_MAP_LAYER_NUMBER];
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct RenderHealth {
	vec3 position;
	float maxHealth;
	float currentHealth;
	unsigned int meshID;
};

struct RenderFont {
	vec3 position;
	GLVM::core::vector<char> font_string;
	float lifeTime;
};

struct SlotData {
	mat4 model;
	vec3 color;
};

struct RenderInventory {
	GLVM::core::vector<SlotData> slotData;
	unsigned int inventoryTextureID;
	unsigned int meshID;
	unsigned int row;
	unsigned int col;
};

struct RenderItem {
	mat4 model;
	unsigned int meshID;
	unsigned int diffuseTexureID;
};

struct RenderCrosshair {
	mat4 model;
	unsigned int meshID;
};

struct RenderCollisionWireframe {
	mat4 model;
	vec3 position;
	float scale;
	GLVM::core::MeshAxisMaxAbsoluteValues meshAxisMaxAbsoluteValues;
};

struct RenderSpacialGrid {
	float chunkSize;
	float halfWidth;
	float halfHeight;
	float halfDepth;
};
