#ifndef ITEM_COMPONENT
#define ITEM_COMPONENT

#include "Vector.hpp"

namespace GLVM::ecs::components
{
	struct ItemSlotType {
		unsigned int width;
		unsigned int height;
	};
	
	struct item {
		core::vector<unsigned int> occupiedSlots; ///< Array that contain entities with inventorySlotComponent
		ItemSlotType itemSlotType;
	};
} // namespace GLVM::ecs::components

#endif
