#ifndef PROJECTILE_ARCHETYPE_HPP
#define PROJECTILE_ARHCETYPE_HPP

#include "Components/ColliderFlagsComponent.hpp"
#include "Components/ProjectileComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/DamageComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Globals.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"

namespace GLVM::ecs::arch {
	struct ProjectileBundle {
		components::projectile projectile;
		components::damage     damage;
		components::material   material;
	};

	constexpr uint32_t PROJECTILE_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::mesh) +
		 sizeof(components::collider) +
		 sizeof(components::colliderFlags) +
		 sizeof(ProjectileBundle));
	
	struct ProjectileArchetype : Archetype {
		components::transform     transforms[PROJECTILE_ARCH_CHUNK_SIZE];
		components::mesh          meshes[PROJECTILE_ARCH_CHUNK_SIZE];
		components::collider      colliders[PROJECTILE_ARCH_CHUNK_SIZE];
		components::colliderFlags colliderFlags[PROJECTILE_ARCH_CHUNK_SIZE];
		ProjectileBundle          projectileBundles[PROJECTILE_ARCH_CHUNK_SIZE];

		ProjectileArchetype() {
			Components[ComponentsIndices::TRANSFORM_COMPONENT]         = transforms;
			Components[ComponentsIndices::MESH_COMPONENT]              = meshes;
			Components[ComponentsIndices::COLLIDER_COMPONENT]          = colliders;
			Components[ComponentsIndices::COLLIDER_FLAGS_COMPONENT]    = colliderFlags;
			Components[ComponentsIndices::PROJECTILE_BUNDLE_COMPONENT] = projectileBundles;

			mask =
				(1ull << ComponentsIndices::TRANSFORM_COMPONENT) |
				(1ull << ComponentsIndices::MESH_COMPONENT) |
				(1ull << ComponentsIndices::COLLIDER_COMPONENT) |
				(1ull << ComponentsIndices::COLLIDER_FLAGS_COMPONENT) |
				(1ull << ComponentsIndices::PROJECTILE_BUNDLE_COMPONENT);
		}
	};
};

#endif
