#ifndef INVENTORY_ARCHETYPE
#define INVENTORY_ARCHETYPE

#include "Components/TransformComponent.hpp"
#include "Components/InventoryComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Globals.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t INVENTORY_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::inventory) +
		 sizeof(components::material));
	
	struct InventoryArchetype {
		components::transform transforms[INVENTORY_ARCH_CHUNK_SIZE];
		components::inventory invetories[INVENTORY_ARCH_CHUNK_SIZE];
		components::material  materials[INVENTORY_ARCH_CHUNK_SIZE];
	};
}; // namespace GLVM::ecs::arch

#endif
