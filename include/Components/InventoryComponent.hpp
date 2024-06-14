#ifndef INVENTORY_COMPONENT
#define INVENTORY_COMPONENT

#include "Components/InventorySlotComponent.hpp"
#include <climits>

namespace GLVM::ecs::components
{
	class inventory {
	public:
		inventory() {
			for ( unsigned int i = 0; i < row; ++i )
				slots[i] = new unsigned int[col];

			for ( unsigned int i = 0; i < row; ++i )
				for ( unsigned int j = 0; j < col; ++j )
					slots[i][j] = -1;
		}

		unsigned int row = 100;
		unsigned int col = 100;
		// int slots[row][col] = {
		// 	{ -1, -1, -1, -1, -1, -1, -1, -1 },
		// 	{ -1, -1, -1, -1, -1, -1, -1, -1 },
		// 	{ -1, -1, -1, -1, -1, -1, -1, -1 },
		// 	{ -1, -1, -1, -1, -1, -1, -1, -1 },
		// 	{ -1, -1, -1, -1, -1, -1, -1, -1 },
		// 	{ -1, -1, -1, -1, -1, -1, -1, -1 },
		// 	{ -1, -1, -1, -1, -1, -1, -1, -1 },
		// 	{ -1, -1, -1, -1, -1, -1, -1, -1 },
		// };

		unsigned int** slots = new unsigned int*[row];   ///< Array with entities contained inventorySlotComponents
		unsigned int entityOwner = UINT_MAX;
	};
}; // namespace GLVM::ecs::components


#endif
