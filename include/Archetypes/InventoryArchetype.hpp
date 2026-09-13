#ifndef INVENTORY_ARCHETYPE
#define INVENTORY_ARCHETYPE

#include "Components/TransformComponent.hpp"
#include "Components/InventoryComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Globals.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t INVENTORY_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::mesh) +
		 sizeof(components::inventory) +
		 sizeof(components::material));
	
	struct InventoryArchetype : Archetype {
		components::transform transforms[INVENTORY_ARCH_CHUNK_SIZE];
		components::mesh      meshes[INVENTORY_ARCH_CHUNK_SIZE];
		components::inventory invetories[INVENTORY_ARCH_CHUNK_SIZE];
		components::material  materials[INVENTORY_ARCH_CHUNK_SIZE];

		InventoryArchetype() {
            registerComponent<ComponentsIndices::TRANSFORM_COMPONENT>(transforms);
            registerComponent<ComponentsIndices::MESH_COMPONENT>(meshes);
            registerComponent<ComponentsIndices::INVENTORY_COMPONENT>(invetories);
            registerComponent<ComponentsIndices::MATERIAL_COMPONENT>(materials);
        }
        std::unique_ptr<Archetype> cloneEmpty() const override { return std::make_unique<InventoryArchetype>(); }
	};
}; // namespace GLVM::ecs::arch

#endif
