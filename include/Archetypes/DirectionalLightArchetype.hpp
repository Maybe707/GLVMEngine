#ifndef DIRECTIONAL_LIGHT_ARCHETYPE
#define DIRECTIONAL_LIGHT_ARCHETYPE

#include "Components/DirectionalLightComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Globals.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t DIRECTIONAL_LIGHT_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::mesh) +
		 sizeof(components::material) +
		 sizeof(components::directionalLight));
	
	struct DirectionalLightArchetype : Archetype {
		components::transform        transforms[DIRECTIONAL_LIGHT_ARCH_CHUNK_SIZE];
		components::mesh             meshes[DIRECTIONAL_LIGHT_ARCH_CHUNK_SIZE];
		components::material         materials[DIRECTIONAL_LIGHT_ARCH_CHUNK_SIZE];
		components::directionalLight directionalLights[DIRECTIONAL_LIGHT_ARCH_CHUNK_SIZE];

		DirectionalLightArchetype() {
            registerComponent<ComponentsIndices::TRANSFORM_COMPONENT>(transforms);
            registerComponent<ComponentsIndices::MESH_COMPONENT>(meshes);
            registerComponent<ComponentsIndices::MATERIAL_COMPONENT>(materials);
            registerComponent<ComponentsIndices::DIRECTIONAL_LIGHT_COMPONENT>(directionalLights);
        }
        std::unique_ptr<Archetype> cloneEmpty() const override { return std::make_unique<DirectionalLightArchetype>(); }
	};
}; // namespace GLVM::ecs::arch

#endif
