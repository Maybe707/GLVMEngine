#ifndef LEVEL_CHUNK_ARCHETYPE
#define LEVEL_CHUNK_ARCHETYPE

#include "Components/ColliderComponent.hpp"
#include "Components/ColliderFlagsComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "TagComponents/LevelChunkTagComponent.hpp"
#include "Globals.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t LEVEL_CHUNK_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::material) +
		 sizeof(components::mesh) +
		 sizeof(components::collider) +
		 sizeof(components::colliderFlags) +
		 sizeof(tagComponents::levelChunkTagComponent));
	
	struct LevelChunkArchetype : Archetype {
		components::transform                 transforms[LEVEL_CHUNK_ARCH_CHUNK_SIZE];
		components::material                  materials[LEVEL_CHUNK_ARCH_CHUNK_SIZE];
		components::mesh                      meshes[LEVEL_CHUNK_ARCH_CHUNK_SIZE];
		components::collider                  colliders[LEVEL_CHUNK_ARCH_CHUNK_SIZE];
		components::colliderFlags             colliderFlags[LEVEL_CHUNK_ARCH_CHUNK_SIZE];
		tagComponents::levelChunkTagComponent levelChunkTagComponents[LEVEL_CHUNK_ARCH_CHUNK_SIZE];

		LevelChunkArchetype() {
			Components[ComponentsIndices::TRANSFORM_COMPONENT]       = transforms;
			Components[ComponentsIndices::MATERIAL_COMPONENT]        = materials;
			Components[ComponentsIndices::MESH_COMPONENT]            = meshes;
			Components[ComponentsIndices::COLLIDER_COMPONENT]        = colliders;
			Components[ComponentsIndices::COLLIDER_FLAGS_COMPONENT]  = colliderFlags;
			Components[ComponentsIndices::LEVEL_CHUNK_TAG_COMPONENT] = levelChunkTagComponents;

			mask =
				(1ull << ecs::arch::ComponentsIndices::TRANSFORM_COMPONENT) |
				(1ull << ecs::arch::ComponentsIndices::MATERIAL_COMPONENT) |
				(1ull << ecs::arch::ComponentsIndices::MESH_COMPONENT) |
				(1ull << ecs::arch::ComponentsIndices::COLLIDER_COMPONENT) |
				(1ull << ecs::arch::ComponentsIndices::COLLIDER_FLAGS_COMPONENT) |
				(1ull << ecs::arch::ComponentsIndices::LEVEL_CHUNK_TAG_COMPONENT);
		}
	};
};

#endif
