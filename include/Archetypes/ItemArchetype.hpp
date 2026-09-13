#ifndef ITEM_ARCHETYPE
#define ITEM_ARCHETYPE

#include "ArchetypeECS/ArchECS_Types.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ColliderFlagsComponent.hpp"
#include "Components/ItemComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/RotationComponent.hpp"
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
		 sizeof(components::rotation) +
		 sizeof(components::move) +
		 sizeof(components::item));
	
	struct ItemArchetype : Archetype {
		components::transform     transforms[ITEM_ARCH_CHUNK_SIZE];
		components::collider      colliders[ITEM_ARCH_CHUNK_SIZE];
		components::colliderFlags colliderFlags[ITEM_ARCH_CHUNK_SIZE];
		components::mesh          meshes[ITEM_ARCH_CHUNK_SIZE];
		components::rigidBody     rigidBodies[ITEM_ARCH_CHUNK_SIZE];
		components::material      materials[ITEM_ARCH_CHUNK_SIZE];
		components::rotation      rotations[ITEM_ARCH_CHUNK_SIZE];
		components::move          moves[ITEM_ARCH_CHUNK_SIZE];
		components::item          items[ITEM_ARCH_CHUNK_SIZE];

		ItemArchetype() {
            registerComponent<ComponentsIndices::TRANSFORM_COMPONENT>(transforms);
            registerComponent<ComponentsIndices::COLLIDER_COMPONENT>(colliders);
            registerComponent<ComponentsIndices::COLLIDER_FLAGS_COMPONENT>(colliderFlags);
            registerComponent<ComponentsIndices::MESH_COMPONENT>(meshes);
            registerComponent<ComponentsIndices::RIGID_BODY_COMPONENT>(rigidBodies);
            registerComponent<ComponentsIndices::MATERIAL_COMPONENT>(materials);
            registerComponent<ComponentsIndices::ROTATION_COMPONENT>(rotations);
            registerComponent<ComponentsIndices::MOVE_COMPONENT>(moves);
            registerComponent<ComponentsIndices::ITEM_COMPONENT>(items);
        }
        std::unique_ptr<Archetype> cloneEmpty() const override { return std::make_unique<ItemArchetype>(); }
	};
}; // namespace GLVM::ecs::arch

#endif
