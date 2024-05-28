#ifndef ITEM_COMPONENT
#define ITEM_COMPONENT

#include "Vector.hpp"

namespace GLVM::ecs::components
{
	struct item {
		core::vector<unsigned int> occupiedSlots;
	};
} // namespace GLVM::ecs::components

#endif
