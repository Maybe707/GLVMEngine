#ifndef PLAYER_ARCHETYPE
#define PLAYER_ARCHETYPE

#include "Components/ColliderComponent.hpp"
#include "Components/ColliderFlagsComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Globals.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t PLAYER_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::beholder) +
		 sizeof(components::collider) +
		 sizeof(components::colliderFlags) +
		 sizeof(components::mesh) +
		 sizeof(components::rigidBody) +
		 sizeof(components::health) +
		 sizeof(components::material));
	
	struct PlayerArchetype {
		components::transform     transforms[PLAYER_ARCH_CHUNK_SIZE];
		components::beholder      beholders[PLAYER_ARCH_CHUNK_SIZE];
		components::collider      colliders[PLAYER_ARCH_CHUNK_SIZE];
		components::colliderFlags colliderFlags[PLAYER_ARCH_CHUNK_SIZE];
		components::mesh          meshs[PLAYER_ARCH_CHUNK_SIZE];
		components::rigidBody     rigidBodies[PLAYER_ARCH_CHUNK_SIZE];
		components::health        health[PLAYER_ARCH_CHUNK_SIZE];
		components::material      materials[PLAYER_ARCH_CHUNK_SIZE];
	};
}; // namespace GLVM::ecs::arch

#endif
