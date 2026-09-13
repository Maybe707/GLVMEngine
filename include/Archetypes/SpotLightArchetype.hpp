#ifndef SPOT_LIGHT_ARCHETYPE
#define SPOT_LIGHT_ARCHETYPE

#include "Components/SpotLightComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Globals.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t SPOT_LIGHT_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::mesh) +
		 sizeof(components::material) +
		 sizeof(components::spotLight));
	
	struct SpotLightArchetype : Archetype {
		components::transform     transforms[SPOT_LIGHT_ARCH_CHUNK_SIZE];
		components::mesh          meshes[SPOT_LIGHT_ARCH_CHUNK_SIZE];
		components::material      materials[SPOT_LIGHT_ARCH_CHUNK_SIZE];
		components::spotLight     spotLights[SPOT_LIGHT_ARCH_CHUNK_SIZE];

		SpotLightArchetype() {
            registerComponent<ComponentsIndices::TRANSFORM_COMPONENT>(transforms);
            registerComponent<ComponentsIndices::MESH_COMPONENT>(meshes);
            registerComponent<ComponentsIndices::MATERIAL_COMPONENT>(materials);
            registerComponent<ComponentsIndices::SPOT_LIGHT_COMPONENT>(spotLights);
        }
        std::unique_ptr<Archetype> cloneEmpty() const override { return std::make_unique<SpotLightArchetype>(); }
	};
}; // namespace GLVM::ecs::arch

#endif
