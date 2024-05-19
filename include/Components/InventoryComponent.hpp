#ifndef INVENTORY_COMPONENT
#define INVENTORY_COMPONENT

#include "Components/ItemComponent.hpp"
namespace GLVM::ecs::components
{
	struct inventory {
		unsigned int array[64];
		unsigned int containedItems = 0;
		unsigned int entityOwner;
	};
}; // namespace GLVM::ecs::components


#endif
