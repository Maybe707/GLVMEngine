#ifndef POINT_LIGHT_ARCHETYPE
#define POINT_LIGHT_ARCHETYPE

#include "Components/PointLightComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Globals.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t POINT_LIGHT_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::mesh) +
		 sizeof(components::material) +
		 sizeof(components::pointLight));
	
	struct PointLightArchetype {
		components::transform     transforms[POINT_LIGHT_ARCH_CHUNK_SIZE];
		components::mesh          meshs[POINT_LIGHT_ARCH_CHUNK_SIZE];
		components::material      materials[POINT_LIGHT_ARCH_CHUNK_SIZE];
		components::pointLight    pointLights[POINT_LIGHT_ARCH_CHUNK_SIZE];
	};
}; // namespace GLVM::ecs::arch


#endif
