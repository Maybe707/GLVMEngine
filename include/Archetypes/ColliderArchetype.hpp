#ifndef COLLIDER_ARCHETYPE
#define COLLIDER_ARCHETYPE

#include "Components/ColliderComponent.hpp"
#include "Components/ColliderFlagsComponent.hpp"
#include "Globals.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t COLLIDER_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(GLVM::ecs::components::collider) +
		 sizeof(GLVM::ecs::components::colliderFlags));
	
	struct ColliderArch {
		components::collider      colliders[COLLIDER_ARCH_CHUNK_SIZE];
		components::colliderFlags colliderFlags[COLLIDER_ARCH_CHUNK_SIZE];
	};
};

#endif
