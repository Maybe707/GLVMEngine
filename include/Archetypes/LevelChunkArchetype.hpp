#ifndef LEVEL_CHUNK_ARCHETYPE
#define LEVEL_CHUNK_ARCHETYPE

#include "ArchetypeECS/ArchECS_Types.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ColliderFlagsComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/RotationComponent.hpp"
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
		 sizeof(components::rotation) +
		 sizeof(tagComponents::levelChunkTagComponent));
	
	struct LevelChunkArchetype : Archetype {
		components::transform                 transforms[LEVEL_CHUNK_ARCH_CHUNK_SIZE];
		components::material                  materials[LEVEL_CHUNK_ARCH_CHUNK_SIZE];
		components::mesh                      meshes[LEVEL_CHUNK_ARCH_CHUNK_SIZE];
		components::collider                  colliders[LEVEL_CHUNK_ARCH_CHUNK_SIZE];
		components::colliderFlags             colliderFlags[LEVEL_CHUNK_ARCH_CHUNK_SIZE];
		components::rotation                  rotations[LEVEL_CHUNK_ARCH_CHUNK_SIZE];
		tagComponents::levelChunkTagComponent levelChunkTagComponents[LEVEL_CHUNK_ARCH_CHUNK_SIZE];

		LevelChunkArchetype() {
            registerComponent<ComponentsIndices::TRANSFORM_COMPONENT>(transforms);
            registerComponent<ComponentsIndices::MATERIAL_COMPONENT>(materials);
            registerComponent<ComponentsIndices::MESH_COMPONENT>(meshes);
            registerComponent<ComponentsIndices::COLLIDER_COMPONENT>(colliders);
            registerComponent<ComponentsIndices::COLLIDER_FLAGS_COMPONENT>(colliderFlags);
            registerComponent<ComponentsIndices::ROTATION_COMPONENT>(rotations);
            registerComponent<ComponentsIndices::LEVEL_CHUNK_TAG_COMPONENT>(levelChunkTagComponents);
        }
        std::unique_ptr<Archetype> cloneEmpty() const override { return std::make_unique<LevelChunkArchetype>(); }
	};
};

#endif
