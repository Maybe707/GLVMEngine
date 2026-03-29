#ifndef ENEMY_ARCHETYPE_HPP
#define ENEMY_ARCHETYPE_HPP

#include "Components/AnimationComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ColliderFlagsComponent.hpp"
#include "Components/EnemyComponent.hpp"
#include "Components/FontComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/StateComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
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
		 sizeof(components::rigidBody));
	
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

		EnemyArchetype() {
			Components[ComponentsIndices::TRANSFORM_COMPONENT]       = transforms;
			Components[ComponentsIndices::ENEMY_COMPONENT]           = enemies;
			Components[ComponentsIndices::STATE_COMPONENT]           = states;
			Components[ComponentsIndices::FONT_COMPONENT]            = fonts;
			Components[ComponentsIndices::ANIMATION_COMPONENT]       = animations;
			Components[ComponentsIndices::MATERIAL_COMPONENT]        = materials;
			Components[ComponentsIndices::MESH_COMPONENT]            = meshes;
			Components[ComponentsIndices::COLLIDER_COMPONENT]        = colliders;
			Components[ComponentsIndices::COLLIDER_FLAGS_COMPONENT]  = colliderFlags;
			Components[ComponentsIndices::HEALTH_COMPONENT]          = health;
			Components[ComponentsIndices::RIGID_BODY_COMPONENT]      = rigidBodies;

			mask =
				(1ull << ComponentsIndices::TRANSFORM_COMPONENT) |
				(1ull << ComponentsIndices::ENEMY_COMPONENT) |
				(1ull << ComponentsIndices::STATE_COMPONENT) |
				(1ull << ComponentsIndices::FONT_COMPONENT) |
				(1ull << ComponentsIndices::ANIMATION_COMPONENT) |
				(1ull << ComponentsIndices::MATERIAL_COMPONENT) |
				(1ull << ComponentsIndices::MESH_COMPONENT) |
				(1ull << ComponentsIndices::COLLIDER_COMPONENT) |
				(1ull << ComponentsIndices::COLLIDER_FLAGS_COMPONENT) |
				(1ull << ComponentsIndices::HEALTH_COMPONENT) |
				(1ull << ComponentsIndices::RIGID_BODY_COMPONENT);
		}
	};
}; // namespace GLVM::ecs::arch

#endif
