#ifndef PROJECTILE_ARCHETYPE
#define PROJECTILE_ARHCETYPE

#include "Components/ProjectileComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/DamageComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Globals.hpp"

namespace GLVM::ecs::arch {
	struct ProjectileBundle {
		components::projectile projectile;
		components::damage     damage;
		components::material   material;
	};

	constexpr uint32_t PROJECTILE_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(GLVM::ecs::components::transform) +
		 sizeof(GLVM::ecs::components::mesh) +
		 sizeof(GLVM::ecs::components::collider) +
		 sizeof(ProjectileBundle));
	
	struct ProjectileArchetype {
		components::transform transforms[PROJECTILE_ARCH_CHUNK_SIZE];
		components::mesh      meshs[PROJECTILE_ARCH_CHUNK_SIZE];
		components::collider  colliders[PROJECTILE_ARCH_CHUNK_SIZE];
		ProjectileBundle      projectileBundles[PROJECTILE_ARCH_CHUNK_SIZE];
	};
};

#endif
