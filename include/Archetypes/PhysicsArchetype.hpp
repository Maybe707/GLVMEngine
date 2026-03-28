#ifndef PHYSICS_ARCHETYPE
#define PHYSICS_ARCHETYPE

#include "Components/TransformComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Globals.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t PHYSICS_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::collider) +
		 sizeof(components::move) +
		 sizeof(components::rigidBody));
	
	struct PhysicsArchetype {
		components::transform transforms[PHYSICS_ARCH_CHUNK_SIZE];
		components::collider  colliders[PHYSICS_ARCH_CHUNK_SIZE];
		components::move      moves[PHYSICS_ARCH_CHUNK_SIZE];
		components::rigidBody rigidBodies[PHYSICS_ARCH_CHUNK_SIZE];
	};
}; // namespace GLVM::ecs::arch

#endif
