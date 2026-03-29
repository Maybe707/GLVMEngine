#ifndef POINT_LIGHT_ARCHETYPE
#define POINT_LIGHT_ARCHETYPE

#include "Components/PointLightComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Globals.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t POINT_LIGHT_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::mesh) +
		 sizeof(components::material) +
		 sizeof(components::pointLight));
	
	struct PointLightArchetype : Archetype {
		components::transform     transforms[POINT_LIGHT_ARCH_CHUNK_SIZE];
		components::mesh          meshes[POINT_LIGHT_ARCH_CHUNK_SIZE];
		components::material      materials[POINT_LIGHT_ARCH_CHUNK_SIZE];
		components::pointLight    pointLights[POINT_LIGHT_ARCH_CHUNK_SIZE];

		PointLightArchetype() {
			Components[ComponentsIndices::TRANSFORM_COMPONENT]       = transforms;
			Components[ComponentsIndices::MESH_COMPONENT]            = meshes;
			Components[ComponentsIndices::MATERIAL_COMPONENT]        = materials;
			Components[ComponentsIndices::POINT_LIGHT_COMPONENT]     = pointLights;

			mask =
				(1ull << ComponentsIndices::TRANSFORM_COMPONENT) |
				(1ull << ComponentsIndices::MESH_COMPONENT) |
				(1ull << ComponentsIndices::MATERIAL_COMPONENT) |
				(1ull << ComponentsIndices::POINT_LIGHT_COMPONENT);
		}
	};
}; // namespace GLVM::ecs::arch


#endif
