#ifndef LEVEL_CHUNK_ARCHETYPE
#define LEVEL_CHUNK_ARCHETYPE

#include "Components/TransformComponent.hpp"
#include "Components/AnimationComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/HudComponent.hpp"
#include "Globals.hpp"


namespace GLVM::ecs::arch {
	constexpr uint32_t LEVEL_CHUNK_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(GLVM::ecs::components::transform) +
		 sizeof(GLVM::ecs::components::animation) +
		 sizeof(GLVM::ecs::components::mesh) +
		 sizeof(GLVM::ecs::components::hud));
	
	struct LevelChunkArch {
		components::transform transform[LEVEL_CHUNK_ARCH_CHUNK_SIZE];
		components::animation animation[LEVEL_CHUNK_ARCH_CHUNK_SIZE];
		components::mesh      mesh[LEVEL_CHUNK_ARCH_CHUNK_SIZE];
		components::hud       hud[LEVEL_CHUNK_ARCH_CHUNK_SIZE];
	};
};

#endif
