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
#include "Components/HealthComponent.hpp"
#include "Components/AttackComponent.hpp"
#include "Components/FontComponent.hpp"
#include "TagComponents/ProjectileTagComponent.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t PROJECTILE_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::mesh) +
		 sizeof(components::collider) +
		 sizeof(components::colliderFlags) +
		 sizeof(components::rotation) +
		 sizeof(ProjectileBundle) +
		 sizeof(components::health) +
		 sizeof(components::attack) +
		 sizeof(components::font) +
		 sizeof(tagComponents::projectileTagComponent));
	
	struct ProjectileArchetype : Archetype {
		components::transform     transforms[PROJECTILE_ARCH_CHUNK_SIZE];
		components::mesh          meshes[PROJECTILE_ARCH_CHUNK_SIZE];
		components::collider      colliders[PROJECTILE_ARCH_CHUNK_SIZE];
		components::colliderFlags colliderFlags[PROJECTILE_ARCH_CHUNK_SIZE];
		components::rotation      rotations[PROJECTILE_ARCH_CHUNK_SIZE];
		ProjectileBundle          projectileBundles[PROJECTILE_ARCH_CHUNK_SIZE];
		components::health        heath[PROJECTILE_ARCH_CHUNK_SIZE];
		components::attack        attacks[PROJECTILE_ARCH_CHUNK_SIZE];
		components::font          fonts[PROJECTILE_ARCH_CHUNK_SIZE];
		tagComponents::projectileTagComponent projectileTagComponents[PROJECTILE_ARCH_CHUNK_SIZE];

		ProjectileArchetype() {
            registerComponent<ComponentsIndices::TRANSFORM_COMPONENT>(transforms);
            registerComponent<ComponentsIndices::MESH_COMPONENT>(meshes);
            registerComponent<ComponentsIndices::COLLIDER_COMPONENT>(colliders);
            registerComponent<ComponentsIndices::COLLIDER_FLAGS_COMPONENT>(colliderFlags);
            registerComponent<ComponentsIndices::ROTATION_COMPONENT>(rotations);
            registerComponent<ComponentsIndices::PROJECTILE_BUNDLE_COMPONENT>(projectileBundles);
            registerComponent<ComponentsIndices::HEALTH_COMPONENT>(heath);
            registerComponent<ComponentsIndices::ATTACK_COMPONENT>(attacks);
            registerComponent<ComponentsIndices::FONT_COMPONENT>(fonts);
            registerComponent<ComponentsIndices::PROJECTILE_TAG_COMPONENT>(projectileTagComponents);
        }
        std::unique_ptr<Archetype> cloneEmpty() const override { return std::make_unique<ProjectileArchetype>(); }
	};
};

#endif
