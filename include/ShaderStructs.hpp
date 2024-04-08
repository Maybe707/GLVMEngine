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

	struct alignas(64) FONT_UBO {
		vec3 position;
	};
} // namespace GLVM::core

#endif
