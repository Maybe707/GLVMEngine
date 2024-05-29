#ifndef ITEM_COMPONENT
#define ITEM_COMPONENT

#include "Vector.hpp"

namespace GLVM::ecs::components
{
	struct item {
		core::vector<unsigned int> occupiedSlots; ///< Array that contain entities with inventorySlotComponent
		unsigned int itemSlotType;
	};
} // namespace GLVM::ecs::components

#endif
