#include "Systems/InventorySystem.hpp"

namespace GLVM::ecs
{
	void InventorySystem::Update() {
		if ( isInventoryOpened ) {
			namespace cm = GLVM::ecs::components;
			ComponentManager* componentManager = GLVM::ecs::ComponentManager::GetInstance();
			
			core::vector<Entity> linkedCrosshairEntities = componentManager->collectLinkedEntities<cm::crosshair, cm::transform>();
			cm::transform* crosshairTransformComponent = componentManager->GetComponent<cm::transform>(linkedCrosshairEntities[0]);

			core::vector<Entity> linkedInventoryEntities = componentManager->collectLinkedEntities<cm::transform, cm::inventory>();
			cm::transform* inventoryTransformComponent = componentManager->GetComponent<cm::transform>(linkedInventoryEntities[0]);
			cm::inventory* inventoryComponent = componentManager->GetComponent<cm::inventory>(linkedInventoryEntities[0]);

			const float inventorySlotScale = inventoryTransformComponent->gltf ? inventoryComponent->slotScale * 2.0f : inventoryComponent->slotScale;
			const float inventorySlotHalfScale = inventoryTransformComponent->gltf ? inventoryComponent->slotScale : inventoryComponent->slotScale * 0.5f;

//			std::cout << "itme draged flag: " << isItemDraged << std::endl;
			// std::cout << "item draged: " << *isItemDraged << std::endl;
			// std::cout << "lkm pressed: " << isLeftMouseButtonPressed << std::endl;
			// std::cout << "lkm released: " << *isLeftMouseButtonReleased << std::endl;
			
			if ( *isItemDraged < 0 && isLeftMouseButtonPressed && *isLeftMouseButtonReleased ) {
				// std::cout << "item draged: " << *isItemDraged << std::endl;
				// std::cout << "lkm pressed: " << isLeftMouseButtonPressed << std::endl;
				// std::cout << "lkm released: " << *isLeftMouseButtonReleased << std::endl;
				
				std::cout << "item taken" << std::endl;
				if ( checkCrosshairInventoryIntersection( crosshairTransformComponent, inventoryTransformComponent, inventoryComponent,
														  inventorySlotScale, inventorySlotHalfScale) ) {
					point2D<int> intersectionSlot = determineActualIntersectionSlot( crosshairTransformComponent, inventoryTransformComponent, inventorySlotScale, inventorySlotHalfScale );
					const unsigned int row    = intersectionSlot.y;
					const unsigned int column = intersectionSlot.x;
					const unsigned int entity = inventoryComponent->slots[row][column];
					if( entity != UINT_MAX && entity >= 0 ) {
						cm::item* itemComponent   = componentManager->GetComponent<cm::item>(entity);
						if( itemComponent != nullptr ) {
							for( unsigned int i = 0; i < itemComponent->occupiedSlots.GetSize(); ++i ) {
								unsigned int row_index = itemComponent->occupiedSlots[i] / inventoryComponent->row;
								unsigned int col_index = itemComponent->occupiedSlots[i] % inventoryComponent->col;

								inventoryComponent->slots[row_index][col_index] = UINT_MAX;
							}
						}
						cm::transform* itemTransformComponent = componentManager->GetComponent<cm::transform>(entity);
						cm::collider* itemColliderComponent = componentManager->GetComponent<cm::collider>(entity);
						if( itemTransformComponent != nullptr && itemColliderComponent != nullptr ) {
//							std::cout << "TEST" << std::endl;
//							itemTransformComponent->position = crosshairTransformComponent->position;
//							itemColliderComponent->itemDrag  = true;
							*isItemDraged = entity;
						}
					}
					
					for( unsigned int i = 0; i < 8; ++i ) {
						for( unsigned int j = 0; j < 8; ++j ) {
							std::cout << inventoryComponent->slots[i][j] << " ";
						}
						std::cout << std::endl;
					}
					// point2D<float> point;
					// std::cout << point << std::endl;
				}
				// std::cout << "Take an item" << std::endl;
				// std::cout << "x: " << mouseOffsetX << std::endl;
				// std::cout << "y: " << mouseOffsetY << std::endl;
				*isLeftMouseButtonReleased = false;
//				isItemDraged = true;
			}

			if ( *isItemDraged >= 0 ) {
//			std::cout << "highlight item slots" << std::endl;
			}

			if ( *isItemDraged >= 0 && isLeftMouseButtonPressed && *isLeftMouseButtonReleased ) {
				if ( checkCrosshairInventoryIntersection( crosshairTransformComponent, inventoryTransformComponent, inventoryComponent,
														  inventorySlotScale, inventorySlotHalfScale) ) {
					[[maybe_unused]] point2D<int> intersectionSlot = determineActualIntersectionSlot( crosshairTransformComponent, inventoryTransformComponent, inventorySlotScale, inventorySlotHalfScale );

					cm::item* itemComponent   = componentManager->GetComponent<cm::item>(*isItemDraged);
					if( itemComponent != nullptr ) {
						const unsigned int itemWidth  = itemComponent->itemSlotType.width;
						const unsigned int itemHeight = itemComponent->itemSlotType.height;

						const unsigned int row    = intersectionSlot.y;
						const unsigned int column = intersectionSlot.x;

						if( itemWidth % 2 == 0 ) {
							const float slotCenterX = inventoryTransformComponent->position[0] + static_cast<float>(column) * inventorySlotScale;
							if( slotCenterX > crosshairTransformComponent->position[0] ) {
								std::cout << "cursor in a left position" << std::endl;
							} else {
								std::cout << "cursor in a right position" << std::endl;
							}
						}
						if( itemHeight % 2 == 0 ) {
							const float slotCenterY = inventoryTransformComponent->position[1] + static_cast<float>(row) * inventorySlotScale * aspectRate;
							if( slotCenterY > crosshairTransformComponent->position[1] ) {
								std::cout << "cursor in an apper position" << std::endl;
							} else {
								std::cout << "cursor in an lower position" << std::endl;
							}
						}

						std::cout << "cross: " << crosshairTransformComponent->position << std::endl;
						// std::cout << "slotX: " << slotX << std::endl;
						// std::cout << "slotY: " << slotY << std::endl;
					}
				}

				std::cout << "item droped" << std::endl;
				*isLeftMouseButtonReleased = false;
//				isItemDraged = false;
				*isItemDraged = -1;
			}
		}
	}

	bool InventorySystem::checkCrosshairInventoryIntersection( components::transform* crosshairTransformComponent, components::transform* inventoryTransformComponent,
															   components::inventory* inventoryComponent, const float inventorySlotScale, const float inventorySlotHalfScale ) {
		if( crosshairTransformComponent->position[0] > inventoryTransformComponent->position[0] - inventorySlotHalfScale &&
			crosshairTransformComponent->position[0] < inventoryTransformComponent->position[0] - inventorySlotHalfScale + inventorySlotScale * inventoryComponent->col &&
			crosshairTransformComponent->position[1] > inventoryTransformComponent->position[1] - inventorySlotHalfScale * aspectRate &&
			crosshairTransformComponent->position[1] < inventoryTransformComponent->position[1] - inventorySlotHalfScale * aspectRate + inventorySlotScale * inventoryComponent->row * aspectRate ) {
//			std::cout << "TEST" << std::endl;
			return true;
		} else {
			return false;
		}
	}

	point2D<int> InventorySystem::determineActualIntersectionSlot( components::transform* crosshairTransformComponent, components::transform* inventoryTransformComponent,
															  const float inventorySlotScale, const float inventorySlotHalfScale ) {
		// std::cout << "cross x: " << crosshairTransformComponent->position[0] << std::endl;
		// std::cout << "cross y: " << crosshairTransformComponent->position[1] << std::endl;
		// std::cout << "slot x: " << inventoryTransformComponent->position[0] << std::endl;
		float x_delta = crosshairTransformComponent->position[0] - inventoryTransformComponent->position[0] + inventorySlotHalfScale;
		float y_delta = crosshairTransformComponent->position[1] - inventoryTransformComponent->position[1] + inventorySlotHalfScale * aspectRate;

		// std::cout << "x delta: " << x_delta << std::endl;
		// std::cout << "y delta: " << y_delta << std::endl;
		
		point2D<int> slotPosition{ (int)(x_delta / inventorySlotScale), (int)(y_delta / (inventorySlotScale * aspectRate)) };
//		point2D<int> slotPosition{ (int)(y_delta / (inventorySlotScale * aspectRate)), (int)(x_delta / inventorySlotScale) };
		
		return slotPosition;
	}
} ///< namespace GLVM::ecs
