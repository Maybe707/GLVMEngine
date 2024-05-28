#include "Systems/ItemSystem.hpp"

namespace GLVM::ecs
{
	/*
	  ===========================================
	  This method is trying to search for suitable
	  slots for the given 2x2 type item. It returns
	  true if it finds them and false otherwise.
	  ==========================================
	*/
	
	bool ItemSystem::putItem2x2(components::inventory* inventoryComponent, unsigned int itemEntity) {
		namespace cm = GLVM::ecs::components;
        ComponentManager* componentManager = GLVM::ecs::ComponentManager::GetInstance();
		
		bool isSlotFound = false;
		unsigned int row = inventoryComponent->row;
		unsigned int col = inventoryComponent->col;
		cm::item* itemComponent = componentManager->GetComponent<cm::item>(itemEntity);
		
		for ( unsigned int i = 0; i < row; ++i )
			for ( unsigned int j = 0; j < col; ++j ) {
				if ( i < row - 1 && j < col - 1 ) {
					cm::inventorySlot* localItemSlot_00 = componentManager->GetComponent<cm::inventorySlot>(inventoryComponent->slots[i][j]);
					cm::inventorySlot* localItemSlot_01 = componentManager->GetComponent<cm::inventorySlot>(inventoryComponent->slots[i][j + 1]);
					cm::inventorySlot* localItemSlot_10 = componentManager->GetComponent<cm::inventorySlot>(inventoryComponent->slots[i + 1][j]);
					cm::inventorySlot* localItemSlot_11 = componentManager->GetComponent<cm::inventorySlot>(inventoryComponent->slots[i + 1][j + 1]); 
				
					if ( localItemSlot_00->itemEntity == UINT_MAX &&
						 localItemSlot_01->itemEntity == UINT_MAX &&
						 localItemSlot_10->itemEntity == UINT_MAX &&
						 localItemSlot_11->itemEntity == UINT_MAX) {
						localItemSlot_00->itemEntity = itemEntity;
						localItemSlot_01->itemEntity = itemEntity;
						localItemSlot_10->itemEntity = itemEntity;
						localItemSlot_11->itemEntity = itemEntity;
						itemComponent->occupiedSlots.Push({ .row = i, .col = j});
						itemComponent->occupiedSlots.Push({ .row = i, .col = j + 1});
						itemComponent->occupiedSlots.Push({ .row = i + 1, .col = j});
						itemComponent->occupiedSlots.Push({ .row = i + 1, .col = j + 1});
						
						isSlotFound = true;
						return isSlotFound;
					}
				}
			}

		return isSlotFound;
	}
	
	void ItemSystem::Update() {
		namespace cm = GLVM::ecs::components;
        ComponentManager* componentManager = GLVM::ecs::ComponentManager::GetInstance();

		core::vector<Entity> inventoryLinkedEntities = componentManager->collectLinkedEntities<cm::inventory>();
		core::vector<Entity> linkedEntities  = componentManager->collectLinkedEntities<cm::item, cm::collider, cm::transform,
																					   cm::rigidBody, cm::actor>();

		for ( unsigned int m = 0; m < inventoryLinkedEntities.GetSize(); ++m ) {
			unsigned int inventoryEntity = inventoryLinkedEntities[m];
		    cm::inventory* inventoryComponent = componentManager->GetComponent<cm::inventory>(inventoryEntity);

			for ( unsigned int i = 0; i < linkedEntities.GetSize(); ++i ) {
				unsigned int itemEntity = linkedEntities[i];
				cm::collider* itemColliderComponent = componentManager->GetComponent<cm::collider>(itemEntity);

				for ( unsigned int j = 0; j < itemColliderComponent->colliders.GetSize(); ++j ) {
					if ( itemColliderComponent->colliders[j] == inventoryComponent->entityOwner ) {
						if ( putItem2x2(inventoryComponent, itemEntity) ) {
							cm::item* itemComponent = componentManager->GetComponent<cm::item>(itemEntity);
							for ( unsigned int m = 0; m < itemComponent->occupiedSlots.GetSize(); ++m ) {
								std::cout << "row: " << itemComponent->occupiedSlots[m].row <<
									" col: " << itemComponent->occupiedSlots[m].col << std::endl;
							}
							
							componentManager->RemoveComponent<cm::actor>(itemEntity);
							componentManager->RemoveComponent<cm::rigidBody>(itemEntity);
						} else {
							std::cout << "No suitable slots for that item in inventory" << std::endl;
						}
					}
				}
			}
		}

		// if(isInventoryOpened) {
		// 	core::vector<Entity> linkedInventoryEntities = componentManager->collectLinkedEntities<cm::inventory>();
		// 	core::vector<Entity> linkedCrosshairEntities = componentManager->collectLinkedEntities<cm::crosshair, cm::transform>();

		// 	cm::transform* crosshairTransformComponent = componentManager->GetComponent<cm::transform>(linkedCrosshairEntities[0]);
			
		// 	for ( unsigned int i = 0; i < linkedInventoryEntities.GetSize(); ++i ) {
		// 		unsigned int entityInventoryContaining = linkedInventoryEntities[i];
		// 		cm::inventory* inventoryComponent = componentManager->GetComponent<cm::inventory>(entityInventoryContaining);

		// 		for ( unsigned int j = 0; j < inventoryComponent->containedItems; ++j ) {
		// 			unsigned int entityItemContaining = inventoryComponent->items[j];
		// 			cm::collider* itemColliderComponent = componentManager->GetComponent<cm::collider>(entityItemContaining);
		// 			cm::transform* itemTransformComponent = componentManager->GetComponent<cm::transform>(entityItemContaining);

		// 			if ( *isLeftMouseButtonReleased && isItemDraged && isLeftMouseButtonPressed && itemColliderComponent->bWall_Collision_ ) {
		// 				isItemDraged = false;
		// 				*isLeftMouseButtonReleased = false;
		// 				itemColliderComponent->itemDrag = false;
		// 				itemColliderComponent->bWall_Collision_ = false;

		// 				return;
		// 			}
					
		// 			if ( itemColliderComponent->bWall_Collision_ ) {
		// 				itemTransformComponent->tPosition = crosshairTransformComponent->tPosition;
		// 				itemColliderComponent->itemDrag = true;
		// 				isItemDraged = true;
		// 			} 
		// 		}
		// 	}
		// }
	}
} // namespace GLVM::ecs
