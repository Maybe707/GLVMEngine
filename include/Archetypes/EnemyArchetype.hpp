#ifndef ENEMY_ARCHETYPE
#define ENEMY_ARCHETYPE

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
	
	struct EnemyArchetype {
		components::transform     transforms[ENEMY_ARCH_CHUNK_SIZE];
		components::enemy         enemies[ENEMY_ARCH_CHUNK_SIZE];
		components::state         states[ENEMY_ARCH_CHUNK_SIZE];
		components::font          fonts[ENEMY_ARCH_CHUNK_SIZE];
		components::animation     animations[ENEMY_ARCH_CHUNK_SIZE];
		components::material      materials[ENEMY_ARCH_CHUNK_SIZE];
		components::mesh          meshs[ENEMY_ARCH_CHUNK_SIZE];
		components::collider      colliders[ENEMY_ARCH_CHUNK_SIZE];
		components::colliderFlags colliderFlags[ENEMY_ARCH_CHUNK_SIZE];
		components::health        health[ENEMY_ARCH_CHUNK_SIZE];
		components::rigidBody     rigidBodies[ENEMY_ARCH_CHUNK_SIZE];
	};
}; // namespace GLVM::ecs::arch

#endif
