#ifndef INVENTORY_ARCHETYPE
#define INVENTORY_ARCHETYPE

#include "Components/TransformComponent.hpp"
#include "Components/InventoryComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Globals.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t INVENTORY_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::inventory) +
		 sizeof(components::material));
	
	struct InventoryArchetype : Archetype {
		components::transform transforms[INVENTORY_ARCH_CHUNK_SIZE];
		components::inventory invetories[INVENTORY_ARCH_CHUNK_SIZE];
		components::material  materials[INVENTORY_ARCH_CHUNK_SIZE];

		InventoryArchetype() {
			Components[ComponentsIndices::TRANSFORM_COMPONENT]       = transforms;
			Components[ComponentsIndices::INVENTORY_COMPONENT]       = invetories;
			Components[ComponentsIndices::MATERIAL_COMPONENT]        = materials;
		}
	};
}; // namespace GLVM::ecs::arch

#endif
