#ifndef STATIC_MESH_ARCHETYPE
#define STATIC_MESH_ARCHETYPE

#include "Components/ColliderComponent.hpp"
#include "Components/ColliderFlagsComponent.hpp"
#include "Components/FontComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Globals.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t PLAYER_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::collider) +
		 sizeof(components::colliderFlags) +
		 sizeof(components::mesh) +
		 sizeof(components::material) +
		 sizeof(components::font));
	
	struct StaticMeshArchetype : Archetype {
		components::transform     transforms[PLAYER_ARCH_CHUNK_SIZE];
		components::collider      colliders[PLAYER_ARCH_CHUNK_SIZE];
		components::colliderFlags colliderFlags[PLAYER_ARCH_CHUNK_SIZE];
		components::mesh          meshes[PLAYER_ARCH_CHUNK_SIZE];
		components::material      materials[PLAYER_ARCH_CHUNK_SIZE];
		components::font          fonts[PLAYER_ARCH_CHUNK_SIZE];

		StaticMeshArchetype() {
			Components[ComponentsIndices::TRANSFORM_COMPONENT]       = transforms;
			Components[ComponentsIndices::COLLIDER_COMPONENT]        = colliders;
			Components[ComponentsIndices::COLLIDER_FLAGS_COMPONENT]  = colliderFlags;
			Components[ComponentsIndices::MESH_COMPONENT]            = meshes;
			Components[ComponentsIndices::MATERIAL_COMPONENT]        = materials;
			Components[ComponentsIndices::FONT_COMPONENT]            = fonts;
		}
	};
}; // namespace GLVM::ecs::arch


#endif
