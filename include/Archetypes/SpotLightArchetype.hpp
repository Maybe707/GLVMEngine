#ifndef SPOT_LIGHT_ARCHETYPE
#define SPOT_LIGHT_ARCHETYPE

#include "Components/SpotLightComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Globals.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t SPOT_LIGHT_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::mesh) +
		 sizeof(components::material) +
		 sizeof(components::spotLight));
	
	struct SpotLightArchetype {
		components::transform     transforms[SPOT_LIGHT_ARCH_CHUNK_SIZE];
		components::mesh          meshs[SPOT_LIGHT_ARCH_CHUNK_SIZE];
		components::material      materials[SPOT_LIGHT_ARCH_CHUNK_SIZE];
		components::spotLight     spotLights[SPOT_LIGHT_ARCH_CHUNK_SIZE];
	};
}; // namespace GLVM::ecs::arch

#endif
