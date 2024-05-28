#ifndef ITEM_COMPONENT
#define ITEM_COMPONENT

#include "Vector.hpp"

namespace GLVM::ecs::components
{
	struct ItemSlots {
		unsigned int row;
		unsigned int col;
	};
	
	struct item {
		core::vector<ItemSlots> occupiedSlots;
		unsigned int itemSlotType;
	};
} // namespace GLVM::ecs::components

#endif
