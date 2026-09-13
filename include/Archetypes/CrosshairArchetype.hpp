#ifndef CROSSHAIR_ARCHETYPE
#define CROSSHAIR_ARCHETYPE

#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "TagComponents/CrosshairTagComponent.hpp"
#include "Globals.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t CROSSHAIR_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::mesh) +
		 sizeof(components::material) +
		 sizeof(tagComponents::crossHairTagComponent));
	
	struct CrosshairArchetype : Archetype {
		components::transform                transforms[CROSSHAIR_ARCH_CHUNK_SIZE];
		components::mesh                     meshes[CROSSHAIR_ARCH_CHUNK_SIZE];
		components::material                 materials[CROSSHAIR_ARCH_CHUNK_SIZE];
		tagComponents::crossHairTagComponent crosshairTagComponents[CROSSHAIR_ARCH_CHUNK_SIZE];

		CrosshairArchetype() {
            registerComponent<ComponentsIndices::TRANSFORM_COMPONENT>(transforms);
            registerComponent<ComponentsIndices::MESH_COMPONENT>(meshes);
            registerComponent<ComponentsIndices::MATERIAL_COMPONENT>(materials);
            registerComponent<ComponentsIndices::CROSSHAIR_TAG_COMPONENT>(crosshairTagComponents);
        }
        std::unique_ptr<Archetype> cloneEmpty() const override { return std::make_unique<CrosshairArchetype>(); }
	};
}; // namespace GLVM::ecs::arch

#endif
