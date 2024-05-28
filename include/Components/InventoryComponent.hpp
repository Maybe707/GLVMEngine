#ifndef INVENTORY_COMPONENT
#define INVENTORY_COMPONENT

#include "Components/InventorySlotComponent.hpp"

namespace GLVM::ecs::components
{
	struct inventory {
		unsigned int slots[8][8];
		unsigned int containedItems = 0;
		unsigned int entityOwner;
	};
}; // namespace GLVM::ecs::components


#endif
