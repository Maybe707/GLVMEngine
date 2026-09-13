#ifndef STATIC_MESH_ARCHETYPE_HPP
#define STATIC_MESH_ARCHETYPE_HPP

#include "Components/ColliderComponent.hpp"
#include "Components/ColliderFlagsComponent.hpp"
#include "Components/FontComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/RotationComponent.hpp"
#include "Globals.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"
#include "TagComponents/StaticMeshTagComponent.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t STATIC_MESH_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::collider) +
		 sizeof(components::colliderFlags) +
		 sizeof(components::mesh) +
		 sizeof(components::material) +
		 sizeof(components::font) +
		 sizeof(components::rotation) +
		 sizeof(tagComponents::staticMeshTagComponent));
	
	struct StaticMeshArchetype : Archetype {
		components::transform                 transforms[STATIC_MESH_ARCH_CHUNK_SIZE];
		components::collider                  colliders[STATIC_MESH_ARCH_CHUNK_SIZE];
		components::colliderFlags             colliderFlags[STATIC_MESH_ARCH_CHUNK_SIZE];
		components::mesh                      meshes[STATIC_MESH_ARCH_CHUNK_SIZE];
		components::material                  materials[STATIC_MESH_ARCH_CHUNK_SIZE];
		components::font                      fonts[STATIC_MESH_ARCH_CHUNK_SIZE];
		components::rotation                  rotations[STATIC_MESH_ARCH_CHUNK_SIZE];
		tagComponents::staticMeshTagComponent staticMeshTagComponents[STATIC_MESH_ARCH_CHUNK_SIZE];

		StaticMeshArchetype() {
            registerComponent<ComponentsIndices::TRANSFORM_COMPONENT>(transforms);
            registerComponent<ComponentsIndices::COLLIDER_COMPONENT>(colliders);
            registerComponent<ComponentsIndices::COLLIDER_FLAGS_COMPONENT>(colliderFlags);
            registerComponent<ComponentsIndices::MESH_COMPONENT>(meshes);
            registerComponent<ComponentsIndices::MATERIAL_COMPONENT>(materials);
            registerComponent<ComponentsIndices::FONT_COMPONENT>(fonts);
            registerComponent<ComponentsIndices::ROTATION_COMPONENT>(rotations);
            registerComponent<ComponentsIndices::STATIC_MESH_TAG_COMPONENT>(staticMeshTagComponents);
        }
        std::unique_ptr<Archetype> cloneEmpty() const override { return std::make_unique<StaticMeshArchetype>(); }
	};
}; // namespace GLVM::ecs::arch


#endif
