#ifndef PROJECTILE_ARCHETYPE_HPP
#define PROJECTILE_ARCHETYPE_HPP

#include "ArchetypeECS/ArchECS_Types.hpp"
#include "Components/ColliderFlagsComponent.hpp"
#include "Components/ProjectileComponent.hpp"
#include "Components/RotationComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/DamageComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Globals.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"
#include "Components/ProjectileBundle.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t PROJECTILE_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::mesh) +
		 sizeof(components::collider) +
		 sizeof(components::colliderFlags) +
		 sizeof(components::rotation) +
		 sizeof(ProjectileBundle));
	
	struct ProjectileArchetype : Archetype {
		components::transform     transforms[PROJECTILE_ARCH_CHUNK_SIZE];
		components::mesh          meshes[PROJECTILE_ARCH_CHUNK_SIZE];
		components::collider      colliders[PROJECTILE_ARCH_CHUNK_SIZE];
		components::colliderFlags colliderFlags[PROJECTILE_ARCH_CHUNK_SIZE];
		components::rotation      rotations[PROJECTILE_ARCH_CHUNK_SIZE];
		ProjectileBundle          projectileBundles[PROJECTILE_ARCH_CHUNK_SIZE];

		ProjectileArchetype() {
			components[ComponentsIndices::TRANSFORM_COMPONENT]         = transforms;
			components[ComponentsIndices::MESH_COMPONENT]              = meshes;
			components[ComponentsIndices::COLLIDER_COMPONENT]          = colliders;
			components[ComponentsIndices::COLLIDER_FLAGS_COMPONENT]    = colliderFlags;
			components[ComponentsIndices::ROTATION_COMPONENT]          = rotations;
			components[ComponentsIndices::PROJECTILE_BUNDLE_COMPONENT] = projectileBundles;

			mask =
				(1ull << ComponentsIndices::TRANSFORM_COMPONENT) |
				(1ull << ComponentsIndices::MESH_COMPONENT) |
				(1ull << ComponentsIndices::COLLIDER_COMPONENT) |
				(1ull << ComponentsIndices::COLLIDER_FLAGS_COMPONENT) |
				(1ull << ComponentsIndices::ROTATION_COMPONENT) |
				(1ull << ComponentsIndices::PROJECTILE_BUNDLE_COMPONENT);

			componentIds[0] = ComponentsIndices::TRANSFORM_COMPONENT;
			componentIds[1] = ComponentsIndices::MESH_COMPONENT;
			componentIds[2] = ComponentsIndices::COLLIDER_COMPONENT;
			componentIds[3] = ComponentsIndices::COLLIDER_FLAGS_COMPONENT;
			componentIds[4] = ComponentsIndices::ROTATION_COMPONENT;
			componentIds[5] = ComponentsIndices::PROJECTILE_BUNDLE_COMPONENT;
			componentCount = 6;
		}
	};
};

#endif
