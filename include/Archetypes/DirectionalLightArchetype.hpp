#ifndef DIRECTIONAL_LIGHT_ARCHETYPE
#define DIRECTIONAL_LIGHT_ARCHETYPE

#include "Components/DirectionalLightComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Globals.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t DIRECTIONAL_LIGHT_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::mesh) +
		 sizeof(components::material) +
		 sizeof(components::directionalLight));
	
	struct DirectionalLightArchetype {
		components::transform     transforms[DIRECTIONAL_LIGHT_ARCH_CHUNK_SIZE];
		components::mesh          meshs[DIRECTIONAL_LIGHT_ARCH_CHUNK_SIZE];
		components::material      materials[DIRECTIONAL_LIGHT_ARCH_CHUNK_SIZE];
		components::directionalLight directionalLights[DIRECTIONAL_LIGHT_ARCH_CHUNK_SIZE];
	};
}; // namespace GLVM::ecs::arch

#endif
