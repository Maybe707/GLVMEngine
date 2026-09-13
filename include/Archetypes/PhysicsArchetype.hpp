#ifndef PHYSICS_ARCHETYPE
#define PHYSICS_ARCHETYPE

#include "Components/ColliderFlagsComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Globals.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t PHYSICS_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::collider) +
		 sizeof(components::colliderFlags) +
		 sizeof(components::move) +
		 sizeof(components::rigidBody));
	
	struct PhysicsArchetype : Archetype {
		components::transform     transforms[PHYSICS_ARCH_CHUNK_SIZE];
		components::collider      colliders[PHYSICS_ARCH_CHUNK_SIZE];
		components::colliderFlags colliderFlags[PHYSICS_ARCH_CHUNK_SIZE];
		components::move          moves[PHYSICS_ARCH_CHUNK_SIZE];
		components::rigidBody     rigidBodies[PHYSICS_ARCH_CHUNK_SIZE];

		PhysicsArchetype() {
            registerComponent<ComponentsIndices::TRANSFORM_COMPONENT>(transforms);
            registerComponent<ComponentsIndices::COLLIDER_COMPONENT>(colliders);
            registerComponent<ComponentsIndices::COLLIDER_FLAGS_COMPONENT>(colliderFlags);
            registerComponent<ComponentsIndices::MOVE_COMPONENT>(moves);
            registerComponent<ComponentsIndices::RIGID_BODY_COMPONENT>(rigidBodies);
        }
        std::unique_ptr<Archetype> cloneEmpty() const override { return std::make_unique<PhysicsArchetype>(); }
	};
}; // namespace GLVM::ecs::arch

#endif
