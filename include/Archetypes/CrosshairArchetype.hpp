#ifndef CROSSHAIR_ARCHETYPE
#define CROSSHAIR_ARCHETYPE

#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Globals.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t CROSSHAIR_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::mesh) +
		 sizeof(components::material));
	
	struct CrosshairArchetype : Archetype {
		components::transform transforms[CROSSHAIR_ARCH_CHUNK_SIZE];
		components::mesh      meshes[CROSSHAIR_ARCH_CHUNK_SIZE];
		components::material  materials[CROSSHAIR_ARCH_CHUNK_SIZE];

		CrosshairArchetype() {
			Components[ComponentsIndices::TRANSFORM_COMPONENT]       = transforms;
			Components[ComponentsIndices::MESH_COMPONENT]            = meshes;
			Components[ComponentsIndices::MATERIAL_COMPONENT]        = materials;

			mask =
				(1ull << ComponentsIndices::TRANSFORM_COMPONENT) |
				(1ull << ComponentsIndices::MESH_COMPONENT) |
				(1ull << ComponentsIndices::MATERIAL_COMPONENT);
		}
	};
}; // namespace GLVM::ecs::arch

#endif
