#include "Systems/InventorySystem.hpp"

namespace GLVM::ecs
{
	void InventorySystem::Update() {
		if ( isInventoryOpened ) {
			if ( !isItemDraged && isLeftMouseButtonPressed && *isLeftMouseButtonReleased ) {
				std::cout << "Take an item" << std::endl;
				std::cout << "x: " << mouseOffsetX << std::endl;
				std::cout << "y: " << mouseOffsetY << std::endl;
				*isLeftMouseButtonReleased = false;
				isItemDraged = true;
			}

			if ( isItemDraged ) {
//			std::cout << "highlight item slots" << std::endl;
			}

			if ( isItemDraged && isLeftMouseButtonPressed && *isLeftMouseButtonReleased ) {
				std::cout << "item droped" << std::endl;
				*isLeftMouseButtonReleased = false;
				isItemDraged = false;
			}
		}
	}
} ///< namespace GLVM::ecs
