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

		unsigned int item_width = itemComponent->itemSlotType.width;
		unsigned int item_height = itemComponent->itemSlotType.height;
		for ( unsigned int i = 0; i < row - item_height + 1; ++i )
			for ( unsigned int j = 0; j < col - item_width + 1; ++j ) {
				core::vector<cm::inventorySlot*> maybeAvailabeSlots;
				core::vector<unsigned int> indicesOfMaybeAvailableSlots;
				for ( unsigned int m = i; m < i + item_height; ++m )
					for ( unsigned int n = j; n < j + item_width; ++n ) {
						maybeAvailabeSlots.Push(componentManager->GetComponent<cm::inventorySlot>(inventoryComponent->slots[m][n]));
//						std::cout << "pointer: " << componentManager->GetComponent<cm::inventorySlot>(inventoryComponent->slots[m][n]) << std::endl;
//						std::cout << "slot entity first entiry: " << inventoryComponent->slots[m][n] << std::endl;
						indicesOfMaybeAvailableSlots.Push(m);
						indicesOfMaybeAvailableSlots.Push(n);
					}
					
				// cm::inventorySlot* localItemSlot_00 = componentManager->GetComponent<cm::inventorySlot>(inventoryComponent->slots[i][j]);
				// cm::inventorySlot* localItemSlot_01 = componentManager->GetComponent<cm::inventorySlot>(inventoryComponent->slots[i][j + 1]);
				// cm::inventorySlot* localItemSlot_10 = componentManager->GetComponent<cm::inventorySlot>(inventoryComponent->slots[i + 1][j]);
				// cm::inventorySlot* localItemSlot_11 = componentManager->GetComponent<cm::inventorySlot>(inventoryComponent->slots[i + 1][j + 1]); 

				// std::cout << "TEST" << std::endl;
				// for ( unsigned int m = 0; m < 4; ++m )
				// 	for ( unsigned int n = 0; n < 8; ++n ) {
				// 		cm::inventorySlot* localItemSlot = componentManager->GetComponent<cm::inventorySlot>(inventoryComponent->slots[m][n]);
				// 		std::cout << "slot value: " << localItemSlot->itemEntity << std::endl;
				// 	}

				unsigned int isAllSlotsAvailable = 0;
				for ( unsigned int v = 0; v < maybeAvailabeSlots.GetSize(); ++v ) {
					if ( maybeAvailabeSlots[v]->itemEntity == UINT_MAX ) {
						// std::cout << "ptr: " << maybeAvailabeSlots[v] << std::endl;
						// std::cout << "TEST 2" << std::endl;
						++isAllSlotsAvailable;
					} else {
						--isAllSlotsAvailable;
					}
				}
				if ( maybeAvailabeSlots.GetSize() == isAllSlotsAvailable ) {
//					std::cout << "size of array: " << maybeAvailabeSlots.GetSize() << std::endl;
					for ( unsigned int w = 0; w < maybeAvailabeSlots.GetSize(); ++w ) {
//						std::cout << "TEST" << std::endl;
						maybeAvailabeSlots[w]->itemEntity = itemEntity;
					}
//					std::cout << "TEST" << std::endl;
					// std::cout << "AFTER TEST" << std::endl;
					// for ( unsigned int m = 0; m < 4; ++m )
					// 	for ( unsigned int n = 0; n < 8; ++n ) {
					// 		cm::inventorySlot* localItemSlot = componentManager->GetComponent<cm::inventorySlot>(inventoryComponent->slots[m][n]);
					// 		std::cout << "slot value: " << localItemSlot->itemEntity << std::endl;
					// 	}

					// for ( unsigned int m = 0; m < item_height; ++m )
					// 	for ( unsigned int n = 0; n < item_width; ++n ) {
					// 		itemComponent->occupiedSlots.Push(inventoryComponent->slots[m][n]);
					// 		std::cout << "slot entity: " << inventoryComponent->slots[m][n] << std::endl;
					// 	}
//					std::cout << "size: " << indicesOfMaybeAvailableSlots.GetSize() << std::endl;
					for ( unsigned int e = 0; e < indicesOfMaybeAvailableSlots.GetSize(); e += 2 ) {
						unsigned int row = indicesOfMaybeAvailableSlots[e];
						unsigned int col = indicesOfMaybeAvailableSlots[e + 1];
						// std::cout << "row: " << row << std::endl;
						// std::cout << "col: " << col << std::endl;
						// std::cout << "slot entity second entiry: " << inventoryComponent->slots[row][col] << std::endl;
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
								cm::item* itemComponent = componentManager->GetComponent<cm::item>(itemEntity);
								for ( unsigned int m = 0; m < itemComponent->occupiedSlots.GetSize(); ++m ) {
//								std::cout << "entity: " << itemComponent->occupiedSlots[m] << std::endl;
								}
							
								componentManager->RemoveComponent<cm::actor>(itemEntity);
								componentManager->RemoveComponent<cm::rigidBody>(itemEntity);
							} else {
//								std::cout << "No suitable slots for that item in inventory" << std::endl;
							}
						}
					}
				}
			}
		}

		if(isInventoryOpened) {
			// namespace cm = GLVM::ecs::components;
			// ComponentManager* componentManager = GLVM::ecs::ComponentManager::GetInstance();

			// core::vector<Entity> inventoryLinkedEntities = componentManager->collectLinkedEntities<cm::inventory>();
			// for ( unsigned int m = 0; m < inventoryLinkedEntities.GetSize(); ++m ) {
			// 	unsigned int inventoryEntity = inventoryLinkedEntities[m];
			// 	cm::inventory* inventoryComponent = componentManager->GetComponent<cm::inventory>(inventoryEntity);
			// 	for ( unsigned int n = 0; n < 8; ++n )
			// 		for ( unsigned int j = 0; j < 8; ++j ) {
			// 			unsigned int inventorySlotEntity = inventoryComponent->slots[n][j];
			// 			cm::inventorySlot* inventorySlotComponent = componentManager->GetComponent<cm::inventorySlot>(inventorySlotEntity);

			// 			std::cout << "item entity in inventory slot component: " << inventorySlotComponent->itemEntity << " for entity: " << inventorySlotEntity << std::endl;
			// 		}
			// }
			
			core::vector<Entity> linkedItemEntities = componentManager->collectLinkedEntities<cm::item, cm::mesh, cm::material, cm::transform, cm::collider>();
			core::vector<Entity> linkedCrosshairEntities = componentManager->collectLinkedEntities<cm::crosshair, cm::transform>();

			cm::transform* crosshairTransformComponent = componentManager->GetComponent<cm::transform>(linkedCrosshairEntities[0]);
			
			for ( unsigned int i = 0; i < linkedItemEntities.GetSize(); ++i ) {
				unsigned int entityItemContaining = linkedItemEntities[i];
//				cm::item* itemComponent = componentManager->GetComponent<cm::item>(entityItemContaining);
				// if ( itemComponent->occupiedSlots.GetSize() == 0 )
				// 	continue;

				cm::collider* itemColliderComponent = componentManager->GetComponent<cm::collider>(entityItemContaining);
				cm::transform* itemTransformComponent = componentManager->GetComponent<cm::transform>(entityItemContaining);

				// if ( *isLeftMouseButtonReleased && isItemDraged && isLeftMouseButtonPressed && itemColliderComponent->bWall_Collision_ ) {
				// 	std::cout << "DROP" << std::endl;
				// 	isItemDraged = false;
				// 	*isLeftMouseButtonReleased = false;
				// 	itemColliderComponent->itemDrag = false;
				// 	itemColliderComponent->bWall_Collision_ = false;

				// 	return;
				// }
//				std::cout << "wall collision " << itemColliderComponent->bWall_Collision_ << std::endl;
				bool isCrosshairCollided = false;
				for ( unsigned int n = 0; n < itemColliderComponent->colliders.GetSize(); ++n ) {
					if ( itemColliderComponent->colliders[n] == linkedCrosshairEntities[0] ) {
						isCrosshairCollided = true;
						break;
					}
				}
				
				if ( itemColliderComponent->bWall_Collision_ && isCrosshairCollided ) {
//					std::cout << "DRAG TO CROSSHAIR" << std::endl;
//					std::cout << "draged entity " << entityItemContaining << std::endl;
					itemTransformComponent->position = crosshairTransformComponent->position;
					itemColliderComponent->itemDrag = true;
					isItemDraged = true;

					// for ( unsigned int j = 0; j < itemComponent->occupiedSlots.GetSize(); ++j ) {
					// 	unsigned int inventorySlotEntity = itemComponent->occupiedSlots[j];
					// 	cm::inventorySlot* inventorySlotComponent = componentManager->GetComponent<cm::inventorySlot>(inventorySlotEntity);
					// 	inventorySlotComponent->itemEntity = UINT_MAX;
					// }

//					itemComponent->occupiedSlots.clear();
				} 
			}
		}
	}
} // namespace GLVM::ecs
