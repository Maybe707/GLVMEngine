#ifndef CROSSHAIR_ARCHETYPE
#define CROSSHAIR_ARCHETYPE

#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Globals.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t CROSSHAIR_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::mesh) +
		 sizeof(components::material));
	
	struct CrosshairArchetype {
		components::transform transforms[CROSSHAIR_ARCH_CHUNK_SIZE];
		components::mesh      meshs[CROSSHAIR_ARCH_CHUNK_SIZE];
		components::material  materials[CROSSHAIR_ARCH_CHUNK_SIZE];
	};
}; // namespace GLVM::ecs::arch

#endif
