#ifndef SHADER_STRUCTS
#define SHADER_STRUCTS

#include "VertexMath.hpp"

namespace GLVM::core {
	struct alignas(64) HUD_UBO {
		mat4 view;
		mat4 proj;
		vec3 entityPosition;
		int isHudExists;
		float maxHP;
		float currentHP;
		float highestY;
	};

	struct alignas(64) HUD_SCREEN_UBO {
		vec3 position;
	};
	
	struct alignas(64) FONT_UBO {
		mat4 view;
		mat4 proj;
		vec3 position;
		float scale;
	};

	struct alignas(64) UI_UBO {
		mat4 model;
	};
} // namespace GLVM::core

#endif
