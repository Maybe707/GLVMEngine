#include "Systems/InventorySystem.hpp"
#include "Components/InventoryComponent.hpp"

namespace GLVM::ecs
{
	void InventorySystem::Update() {
		if ( isInventoryOpened ) {
			namespace cm = GLVM::ecs::components;
			ComponentManager* componentManager = GLVM::ecs::ComponentManager::GetInstance();
			
			core::vector<Entity> linkedCrosshairEntities = componentManager->collectLinkedEntities<cm::crosshair, cm::transform>();
			[[maybe_unused]] cm::transform* crosshairTransformComponent = componentManager->GetComponent<cm::transform>(linkedCrosshairEntities[0]);

			core::vector<Entity> linkedInventoryEntities = componentManager->collectLinkedEntities<cm::transform, cm::inventory>();
			[[maybe_unused]] cm::transform* inventoryTransformComponent = componentManager->GetComponent<cm::transform>(linkedInventoryEntities[0]);
			[[maybe_unused]] cm::inventory* inventoryComponent = componentManager->GetComponent<cm::inventory>(linkedInventoryEntities[0]);

			if ( !isItemDraged && isLeftMouseButtonPressed && *isLeftMouseButtonReleased ) {
				const float inventorySlotScale = inventoryTransformComponent->gltf ? inventoryComponent->slotScale * 2.0f : inventoryComponent->slotScale;
				if( crosshairTransformComponent->position[0] > inventoryTransformComponent->position[0] &&
					crosshairTransformComponent->position[0] < inventoryTransformComponent->position[0] + inventorySlotScale * inventoryComponent->col &&
					crosshairTransformComponent->position[1] > inventoryTransformComponent->position[1] &&
					crosshairTransformComponent->position[1] < inventoryTransformComponent->position[1] + inventorySlotScale * inventoryComponent->row * aspectRate ) {
					std::cout << "TEST" << std::endl;
				}
				// std::cout << "Take an item" << std::endl;
				// std::cout << "x: " << mouseOffsetX << std::endl;
				// std::cout << "y: " << mouseOffsetY << std::endl;
				*isLeftMouseButtonReleased = false;
				isItemDraged = true;
			}

			if ( isItemDraged ) {
//			std::cout << "highlight item slots" << std::endl;
			}

			if ( isItemDraged && isLeftMouseButtonPressed && *isLeftMouseButtonReleased ) {
//				std::cout << "item droped" << std::endl;
				*isLeftMouseButtonReleased = false;
				isItemDraged = false;
			}
		}
	}
} ///< namespace GLVM::ecs
