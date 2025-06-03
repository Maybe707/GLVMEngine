#include "Systems/ItemSystem.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/CrosshairComponent.hpp"
#include "Components/InventoryComponent.hpp"
#include "Components/InventorySlotComponent.hpp"
#include "Components/ItemComponent.hpp"
#include <unistd.h>

namespace GLVM::ecs
{
	/*
	  ===========================================
	  This method is trying to search for suitable
	  slots for the given specific type item. It returns
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

		unsigned int item_width = itemComponent->itemSlotType.width;
		unsigned int item_height = itemComponent->itemSlotType.height;
		for ( unsigned int i = 0; i < row - item_height + 1; ++i )
			for ( unsigned int j = 0; j < col - item_width + 1; ++j ) {
				core::vector<cm::inventorySlot*> maybeAvailabeSlots;
				core::vector<unsigned int> indicesOfMaybeAvailableSlots;
				for ( unsigned int m = i; m < i + item_height; ++m )
					for ( unsigned int n = j; n < j + item_width; ++n ) {
						maybeAvailabeSlots.Push(componentManager->GetComponent<cm::inventorySlot>(inventoryComponent->slots[m][n]));
						indicesOfMaybeAvailableSlots.Push(m);                              ///< Save row index
						indicesOfMaybeAvailableSlots.Push(n);                              ///< Save col index
					}

				unsigned int isAllSlotsAvailable = 0;
				for ( unsigned int v = 0; v < maybeAvailabeSlots.GetSize(); ++v ) {
					if ( maybeAvailabeSlots[v]->itemEntity == UINT_MAX ) {
						++isAllSlotsAvailable;
					} else {
						--isAllSlotsAvailable;
					}
				}
				if ( maybeAvailabeSlots.GetSize() == isAllSlotsAvailable ) {
					for ( unsigned int w = 0; w < maybeAvailabeSlots.GetSize(); ++w ) {
						maybeAvailabeSlots[w]->itemEntity = itemEntity;
					}

					for ( unsigned int e = 0; e < indicesOfMaybeAvailableSlots.GetSize(); e += 2 ) {
						unsigned int row = indicesOfMaybeAvailableSlots[e];                ///< First one is row
						unsigned int col = indicesOfMaybeAvailableSlots[e + 1];            ///< Second one is col

						itemComponent->occupiedSlots.Push(inventoryComponent->slots[row][col]);
					}
					
					isSlotFound = true;
					return isSlotFound;
				}
			}

		return isSlotFound;
	}
	
	void ItemSystem::Update() {
		namespace cm = GLVM::ecs::components;
        ComponentManager* componentManager = GLVM::ecs::ComponentManager::GetInstance();

		if (!isInventoryOpened) {
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
								componentManager->RemoveComponent<cm::actor>(itemEntity);
								componentManager->RemoveComponent<cm::rigidBody>(itemEntity);
							} else {
								std::cout << "No suitable slots for that item in inventory" << std::endl;
							}
						}
					}
				}
			}
		}

		// if(isInventoryOpened) {
		// 	core::vector<Entity> linkedItemEntities = componentManager->collectLinkedEntities<cm::item, cm::mesh, cm::material, cm::transform, cm::collider>();
		// 	core::vector<Entity> linkedCrosshairEntities = componentManager->collectLinkedEntities<cm::crosshair, cm::transform>();
		// 	cm::transform* crosshairTransformComponent = componentManager->GetComponent<cm::transform>(linkedCrosshairEntities[0]);
			
		// 	for ( unsigned int i = 0; i < linkedItemEntities.GetSize(); ++i ) {
		// 		unsigned int entityItemContaining = linkedItemEntities[i];
		// 		cm::collider* itemColliderComponent = componentManager->GetComponent<cm::collider>(entityItemContaining);
		// 		cm::transform* itemTransformComponent = componentManager->GetComponent<cm::transform>(entityItemContaining);

		// 		bool isCrosshairCollided = false;
		// 		for ( unsigned int n = 0; n < itemColliderComponent->colliders.GetSize(); ++n ) {
		// 			if ( itemColliderComponent->colliders[n] == linkedCrosshairEntities[0] ) {                ///< Is there a crosshair among the colliders
		// 				isCrosshairCollided = true;
		// 				break;
		// 			}
		// 		}
				
		// 		if ( itemColliderComponent->wallCollision && isCrosshairCollided ) {
		// 			itemTransformComponent->position = crosshairTransformComponent->position;                 ///< Set crosshair position to draged item
		// 			itemColliderComponent->itemDrag = true;
		// 			isItemDraged = true;
		// 		} 
		// 	}
		// }

		// if ( mouseOffsetX > 1.0f )
		// 	mouseOffsetX = 1.0f;
		// else if ( mouseOffsetX < -1.0f )
		// 	mouseOffsetX = -1.0f;
		
		// if ( mouseOffsetY > 1.0f )
		// 	mouseOffsetY = 1.0f;
		// else if ( mouseOffsetY < -1.0f )
		// 	mouseOffsetY = -1.0f;

		
// 		if ( isInventoryOpened ) {
// 			if ( !isItemDraged && isLeftMouseButtonPressed && *isLeftMouseButtonReleased ) {
// 				std::cout << "Take an item" << std::endl;
// 				std::cout << "x: " << mouseOffsetX << std::endl;
// 				std::cout << "y: " << mouseOffsetY << std::endl;
// 				*isLeftMouseButtonReleased = false;
// 				isItemDraged = true;
// 			}

// 			if ( isItemDraged ) {
// //			std::cout << "highlight item slots" << std::endl;
// 			}

// 			if ( isItemDraged && isLeftMouseButtonPressed && *isLeftMouseButtonReleased ) {
// 				std::cout << "item droped" << std::endl;
// 				*isLeftMouseButtonReleased = false;
// 				isItemDraged = false;
// 			}
// 		}
	}
} // namespace GLVM::ecs
