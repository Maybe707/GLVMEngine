#include "Systems/InventorySystem.hpp"
#include "Components/InventoryComponent.hpp"

namespace GLVM::ecs
{
	void InventorySystem::Update() {
		if ( isInventoryOpened ) {

			if ( !isItemDraged && isLeftMouseButtonPressed && *isLeftMouseButtonReleased ) {
				if ( checkCrosshairInventoryIntersection() ) {
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

	bool InventorySystem::checkCrosshairInventoryIntersection() {
		namespace cm = GLVM::ecs::components;
		ComponentManager* componentManager = GLVM::ecs::ComponentManager::GetInstance();
			
		core::vector<Entity> linkedCrosshairEntities = componentManager->collectLinkedEntities<cm::crosshair, cm::transform>();
		cm::transform* crosshairTransformComponent = componentManager->GetComponent<cm::transform>(linkedCrosshairEntities[0]);

		core::vector<Entity> linkedInventoryEntities = componentManager->collectLinkedEntities<cm::transform, cm::inventory>();
		cm::transform* inventoryTransformComponent = componentManager->GetComponent<cm::transform>(linkedInventoryEntities[0]);
		cm::inventory* inventoryComponent = componentManager->GetComponent<cm::inventory>(linkedInventoryEntities[0]);

		const float inventorySlotScale = inventoryTransformComponent->gltf ? inventoryComponent->slotScale * 2.0f : inventoryComponent->slotScale;
		const float inventorySlotHalfScale = inventoryTransformComponent->gltf ? inventoryComponent->slotScale : inventoryComponent->slotScale * 0.5f;
		if( crosshairTransformComponent->position[0] > inventoryTransformComponent->position[0] - inventorySlotHalfScale &&
			crosshairTransformComponent->position[0] < inventoryTransformComponent->position[0] - inventorySlotHalfScale + inventorySlotScale * inventoryComponent->col &&
			crosshairTransformComponent->position[1] > inventoryTransformComponent->position[1] - inventorySlotHalfScale * aspectRate &&
			crosshairTransformComponent->position[1] < inventoryTransformComponent->position[1] - inventorySlotHalfScale * aspectRate + inventorySlotScale * inventoryComponent->row * aspectRate ) {
			std::cout << "TEST" << std::endl;
			return true;
		} else {
			return false;
		}
	}
} ///< namespace GLVM::ecs
