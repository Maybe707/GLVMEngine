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
#include "ArchetypeECS/ArchetypeInterface.hpp"

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
	
	struct ItemArchetype : Archetype {
		components::transform     transforms[ITEM_ARCH_CHUNK_SIZE];
		components::collider      colliders[ITEM_ARCH_CHUNK_SIZE];
		components::colliderFlags colliderFlags[ITEM_ARCH_CHUNK_SIZE];
		components::mesh          meshes[ITEM_ARCH_CHUNK_SIZE];
		components::rigidBody     rigidBodies[ITEM_ARCH_CHUNK_SIZE];
		components::material      materials[ITEM_ARCH_CHUNK_SIZE];
		components::item          items[ITEM_ARCH_CHUNK_SIZE];

		ItemArchetype() {
			Components[ComponentsIndices::TRANSFORM_COMPONENT]       = transforms;
			Components[ComponentsIndices::COLLIDER_COMPONENT]        = colliders;
			Components[ComponentsIndices::COLLIDER_FLAGS_COMPONENT]  = colliderFlags;
			Components[ComponentsIndices::MESH_COMPONENT]            = meshes;
			Components[ComponentsIndices::RIGID_BODY_COMPONENT]      = rigidBodies;
			Components[ComponentsIndices::MATERIAL_COMPONENT]        = materials;
			Components[ComponentsIndices::ITEM_COMPONENT]            = items;
		}
	};
}; // namespace GLVM::ecs::arch

#endif
