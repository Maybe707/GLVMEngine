#ifndef ITEM_ARCHETYPE
#define ITEM_ARCHETYPE

#include "Components/ColliderComponent.hpp"
#include "Components/ColliderFlagsComponent.hpp"
#include "Components/ItemComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Globals.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t ITEM_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::collider) +
		 sizeof(components::colliderFlags) +
		 sizeof(components::mesh) +
		 sizeof(components::rigidBody) +
		 sizeof(components::material) +
		 sizeof(components::item));
	
	struct ItemArchetype {
		components::transform     transforms[ITEM_ARCH_CHUNK_SIZE];
		components::collider      colliders[ITEM_ARCH_CHUNK_SIZE];
		components::colliderFlags colliderFlags[ITEM_ARCH_CHUNK_SIZE];
		components::mesh          meshs[ITEM_ARCH_CHUNK_SIZE];
		components::rigidBody     rigidBodies[ITEM_ARCH_CHUNK_SIZE];
		components::material      materials[ITEM_ARCH_CHUNK_SIZE];
		components::item          items[ITEM_ARCH_CHUNK_SIZE];
	};
}; // namespace GLVM::ecs::arch

#endif
