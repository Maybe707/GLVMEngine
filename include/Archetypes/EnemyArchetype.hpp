#ifndef ENEMY_ARCHETYPE_HPP
#define ENEMY_ARCHETYPE_HPP

#include "ArchetypeECS/ArchECS_Types.hpp"
#include "Components/AnimationComponent.hpp"
#include "Components/AttackComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ColliderFlagsComponent.hpp"
#include "Components/EnemyComponent.hpp"
#include "Components/FontComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/RotationComponent.hpp"
#include "Components/StateComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Globals.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t ENEMY_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::enemy) +
		 sizeof(components::state) +
		 sizeof(components::font) +
		 sizeof(components::animation) +
		 sizeof(components::material) +
		 sizeof(components::mesh) +
		 sizeof(components::collider) +
		 sizeof(components::colliderFlags) +
		 sizeof(components::health) +
		 sizeof(components::rigidBody) +
		 sizeof(components::attack) +
		 sizeof(components::rotation) +
		 sizeof(components::move));
	
	struct EnemyArchetype : Archetype {
		components::transform     transforms[ENEMY_ARCH_CHUNK_SIZE];
		components::enemy         enemies[ENEMY_ARCH_CHUNK_SIZE];
		components::state         states[ENEMY_ARCH_CHUNK_SIZE];
		components::font          fonts[ENEMY_ARCH_CHUNK_SIZE];
		components::animation     animations[ENEMY_ARCH_CHUNK_SIZE];
		components::material      materials[ENEMY_ARCH_CHUNK_SIZE];
		components::mesh          meshes[ENEMY_ARCH_CHUNK_SIZE];
		components::collider      colliders[ENEMY_ARCH_CHUNK_SIZE];
		components::colliderFlags colliderFlags[ENEMY_ARCH_CHUNK_SIZE];
		components::health        health[ENEMY_ARCH_CHUNK_SIZE];
		components::rigidBody     rigidBodies[ENEMY_ARCH_CHUNK_SIZE];
		components::attack        attacks[ENEMY_ARCH_CHUNK_SIZE];
		components::rotation      rotations[ENEMY_ARCH_CHUNK_SIZE];
		components::move          moves[ENEMY_ARCH_CHUNK_SIZE];

		EnemyArchetype() {
            registerComponent<ComponentsIndices::TRANSFORM_COMPONENT>(transforms);
            registerComponent<ComponentsIndices::ENEMY_COMPONENT>(enemies);
            registerComponent<ComponentsIndices::STATE_COMPONENT>(states);
            registerComponent<ComponentsIndices::FONT_COMPONENT>(fonts);
            registerComponent<ComponentsIndices::ANIMATION_COMPONENT>(animations);
            registerComponent<ComponentsIndices::MATERIAL_COMPONENT>(materials);
            registerComponent<ComponentsIndices::MESH_COMPONENT>(meshes);
            registerComponent<ComponentsIndices::COLLIDER_COMPONENT>(colliders);
            registerComponent<ComponentsIndices::COLLIDER_FLAGS_COMPONENT>(colliderFlags);
            registerComponent<ComponentsIndices::HEALTH_COMPONENT>(health);
            registerComponent<ComponentsIndices::RIGID_BODY_COMPONENT>(rigidBodies);
            registerComponent<ComponentsIndices::ATTACK_COMPONENT>(attacks);
            registerComponent<ComponentsIndices::ROTATION_COMPONENT>(rotations);
            registerComponent<ComponentsIndices::MOVE_COMPONENT>(moves);
        }
        std::unique_ptr<Archetype> cloneEmpty() const override { return std::make_unique<EnemyArchetype>(); }
	};
}; // namespace GLVM::ecs::arch

#endif
