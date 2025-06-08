#include "Systems/InventorySystem.hpp"
#include "Components/InventoryComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "VertexMath.hpp"
                                
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

			if ( *isItemDraged < 0 && isLeftMouseButtonPressed && *isLeftMouseButtonReleased ) {           ///< Take an item from inventory
				if ( checkCrosshairInventoryIntersection( crosshairTransformComponent, inventoryTransformComponent, inventoryComponent,
														  inventorySlotScale, inventorySlotHalfScale) ) {
					point2D<int> intersectionSlot = determineActualIntersectionSlot( crosshairTransformComponent, inventoryTransformComponent, inventorySlotScale, inventorySlotHalfScale );
					const unsigned int row    = intersectionSlot.y;
					const unsigned int column = intersectionSlot.x;
					const unsigned int entity = inventoryComponent->slots[row][column];
					if( entity != UINT_MAX && entity >= 0 ) {  ///< Check slot is not empty and hold an item
						cm::item* itemComponent   = componentManager->GetComponent<cm::item>(entity);
						if( itemComponent != nullptr ) {
							for( unsigned int i = 0; i < itemComponent->occupiedSlots.GetSize(); ++i ) {
								unsigned int row_index = itemComponent->occupiedSlots[i] / inventoryComponent->row;
								unsigned int col_index = itemComponent->occupiedSlots[i] % inventoryComponent->col;

								inventoryComponent->slots[row_index][col_index] = UINT_MAX;   ///< Need to free all slots that hold an item
							}
						}
						*isItemDraged = entity;                                               ///< Set curretly draged item entity
					}
					
					// for( unsigned int i = 0; i < 8; ++i ) {
					// 	for( unsigned int j = 0; j < 8; ++j ) {
					// 		std::cout << inventoryComponent->slots[i][j] << " ";
					// 	}
					// 	std::cout << std::endl;
					// }
				}
				*isLeftMouseButtonReleased = false;
			}

			if ( *isItemDraged >= 0 ) {
//			std::cout << "highlight item slots" << std::endl;
				if ( checkCrosshairInventoryIntersection( crosshairTransformComponent, inventoryTransformComponent, inventoryComponent,
														  inventorySlotScale, inventorySlotHalfScale) ) {
					[[maybe_unused]] point2D<int> intersectionSlot = determineActualIntersectionSlot( crosshairTransformComponent, inventoryTransformComponent, inventorySlotScale, inventorySlotHalfScale );

					cm::item* itemComponent   = componentManager->GetComponent<cm::item>(*isItemDraged);
					core::vector<unsigned int> potentialOccupiedSlots;
					determineSwappableStatusAndSlots( itemComponent, inventoryTransformComponent, potentialOccupiedSlots, crosshairTransformComponent,
													  intersectionSlot, inventoryComponent, inventorySlotScale );
					
					inventoryComponent->highlightedSlots = potentialOccupiedSlots;
					potentialOccupiedSlots.Print();
					inventoryComponent->isAvailableHighlightedSlots = true;
				}
			} else {
				inventoryComponent->highlightedSlots.clear();
				inventoryComponent->isAvailableHighlightedSlots = false;
			}

			if ( *isItemDraged >= 0 && isLeftMouseButtonPressed && *isLeftMouseButtonReleased ) {    ///< Item drop to inventory, swaped or we just cant place
				int isSwapable = 0;
				if ( checkCrosshairInventoryIntersection( crosshairTransformComponent, inventoryTransformComponent, inventoryComponent,
														  inventorySlotScale, inventorySlotHalfScale) ) {
					[[maybe_unused]] point2D<int> intersectionSlot = determineActualIntersectionSlot( crosshairTransformComponent, inventoryTransformComponent, inventorySlotScale, inventorySlotHalfScale );
					cm::item* itemComponent   = componentManager->GetComponent<cm::item>(*isItemDraged);
					core::vector<unsigned int> potentialOccupiedSlots;
					isSwapable = determineSwappableStatusAndSlots( itemComponent, inventoryTransformComponent, potentialOccupiedSlots, crosshairTransformComponent,
																   intersectionSlot, inventoryComponent, inventorySlotScale );

					const int itemWidth  = itemComponent->itemSlotType.width;
					const int itemHeight = itemComponent->itemSlotType.height;

					if( isSwapable == -1 ) {               ///< Default value. Just drop item to all empty slots
						itemComponent->occupiedSlots = potentialOccupiedSlots;
						fillInventorySlots( itemComponent, itemWidth, itemHeight, inventoryComponent, *isItemDraged );
					} else if ( isSwapable > 0 ) {         ///< Swap one item that we draging to another one in inventory
						cm::item* swapedItemComponent = componentManager->GetComponent<cm::item>( isSwapable );
						itemComponent->occupiedSlots = potentialOccupiedSlots;
						fillInventorySlots( swapedItemComponent, swapedItemComponent->itemSlotType.width, swapedItemComponent->itemSlotType.height,
											inventoryComponent, UINT_MAX );
							
						swapedItemComponent->occupiedSlots.clear();
						fillInventorySlots( itemComponent, itemWidth, itemHeight, inventoryComponent, *isItemDraged );
					}

					if( isSwapable == -1 ) {
						*isLeftMouseButtonReleased = false;
						*isItemDraged = -1;
					} else if( isSwapable == -2) {         ///< Already have 2 or more items in potential inventory slots
						*isLeftMouseButtonReleased = false;
					} else {
						*isLeftMouseButtonReleased = false;
						*isItemDraged = isSwapable;
					}
				} else {       ///< Item drop to the ground
					std::cout << "is item draged: " << *isItemDraged << std::endl;
					componentManager->CreateComponent<cm::actor>(*isItemDraged);
					componentManager->CreateComponent<cm::rigidBody>(*isItemDraged);
					*componentManager->GetComponent<cm::rigidBody>(*isItemDraged) = { .fMass_ = 2.0f };
					core::vector<unsigned int> playerEntities = componentManager->collectLinkedEntities<cm::controller>();
					cm::transform* playerTransform = componentManager->GetComponent<cm::transform>(playerEntities[0]);
					cm::transform* itemTransform   = componentManager->GetComponent<cm::transform>(*isItemDraged);
					itemTransform->position = playerTransform->position;
					vec3 normalizedForward = Normalize(playerTransform->forward);
					itemTransform->position[0] += normalizedForward[0] * 2.5f;
					itemTransform->position[1] += normalizedForward[1] * 2.5f;
					itemTransform->position[2] += normalizedForward[2] * 2.5f;
					itemTransform->scale = 0.05f;

					*isItemDraged = -1;
					*isLeftMouseButtonReleased = false;
				}
			}
		}
	}

	int InventorySystem::determineSwappableStatusAndSlots( components::item* itemComponent, components::transform* inventoryTransformComponent,
														   core::vector<unsigned int>& potentialOccupiedSlots, components::transform* crosshairTransformComponent,
														   point2D<int> intersectionSlot, components::inventory* inventoryComponent, const float inventorySlotScale ) {
		if( itemComponent != nullptr ) {
			const int itemWidth  = itemComponent->itemSlotType.width;
			const int itemHeight = itemComponent->itemSlotType.height;

			const int row    = intersectionSlot.y;
			const int column = intersectionSlot.x;

			/// Find left-upper pivot slot inventory
			int rowBasicOffset    = 0;
			int columnBasicOffset = 0;

			/*
			  ===============================================
			  Set as pivot point slot in left upper corner.
			  Need to calculate offset for row and column
			  to change it from center. And need to It is
			  necessary to take into account the offset
			  relative to the center for additional correction
			  ===============================================
			*/
			columnBasicOffset = calculateBasicOffset( itemWidth, inventoryTransformComponent->position[0],
													  crosshairTransformComponent->position[0], column, inventorySlotScale );
			rowBasicOffset    = calculateBasicOffset( itemHeight, inventoryTransformComponent->position[1],
													  crosshairTransformComponent->position[1], row, inventorySlotScale * aspectRate );

			int pivotRow    = row - rowBasicOffset;
			int pivotColumn = column - columnBasicOffset;

			clamp<int>( 0, pivotRow, static_cast<int>(inventoryComponent->row) - itemHeight );
			clamp<int>( 0, pivotColumn, static_cast<int>(inventoryComponent->col) - itemWidth );

			return determineSwappableField( itemComponent, itemWidth, itemHeight, pivotRow, pivotColumn, inventoryComponent, potentialOccupiedSlots );
		} else {
			return -3; ///< Return -3 as error code means itemComponent is nullptr
		}
	}
	
	void InventorySystem::fillInventorySlots( components::item* itemComponent, const int itemWidth, const int itemHeight,
											  components::inventory* inventoryComponent, const int fillValue ) {
		for( int i = 0; i < itemHeight; ++i ) {
			for( int j = 0; j < itemWidth; ++j ) {
				const unsigned int slotsRow = itemComponent->occupiedSlots[i * itemWidth + j] / inventoryComponent->col;
				const unsigned int slotsColumn = itemComponent->occupiedSlots[i * itemWidth + j] % inventoryComponent->col;

				inventoryComponent->slots[slotsRow][slotsColumn] = fillValue;
			}
		}
	}
	
	int InventorySystem::determineSwappableField( components::item* itemComponent, const int itemWidth, const int itemHeight,
												  int pivotRow, int pivotColumn, components::inventory* inventoryComponent,
												  core::vector<unsigned int>& potentialOccupiedSlots) {
		itemComponent->occupiedSlots.clear();
		int isSwapable = -1;                   ///< -1: default value. -2: found two entities in potential slots. Any other value: swapable.
		for( int i = 0; i < itemHeight; ++i ) {
			for( int j = 0; j < itemWidth; ++j ) {
				const unsigned int finalRow    = pivotRow + i;
				const unsigned int finalColumn = pivotColumn + j;
				if ( isSwapable == -1 && inventoryComponent->slots[finalRow][finalColumn] != UINT_MAX ) {
					isSwapable = inventoryComponent->slots[finalRow][finalColumn];
				} else if ( isSwapable > 0 && inventoryComponent->slots[finalRow][finalColumn] != UINT_MAX
							&& (int)inventoryComponent->slots[finalRow][finalColumn] != isSwapable ) {
					isSwapable = -2;
				}
								
				potentialOccupiedSlots.Push(finalRow * inventoryComponent->col + finalColumn);
			}
		}
		
		return isSwapable;
	}
	
	int InventorySystem::calculateBasicOffset( const int itemAxisSize, const float axisValue,
											   const float crosshairAxisPosition, const int axisSlotIndex,
											   const float inventorySlotScale) {
		if( itemAxisSize % 2 == 0 ) {
			const float slotCenterX = axisValue + static_cast<float>(axisSlotIndex) * inventorySlotScale;
			if( slotCenterX > crosshairAxisPosition ) {
				return itemAxisSize / 2;
			}
			return itemAxisSize / 2 - 1;
		}
		return itemAxisSize / 2;
	}
	
	bool InventorySystem::checkCrosshairInventoryIntersection( components::transform* crosshairTransformComponent, components::transform* inventoryTransformComponent,
															   components::inventory* inventoryComponent, const float inventorySlotScale, const float inventorySlotHalfScale ) {
		return crosshairTransformComponent->position[0] > inventoryTransformComponent->position[0] - inventorySlotHalfScale &&
			crosshairTransformComponent->position[0] < inventoryTransformComponent->position[0] - inventorySlotHalfScale + inventorySlotScale * inventoryComponent->col &&
													   crosshairTransformComponent->position[1] > inventoryTransformComponent->position[1] - inventorySlotHalfScale * aspectRate &&
			crosshairTransformComponent->position[1] < inventoryTransformComponent->position[1] - inventorySlotHalfScale * aspectRate + inventorySlotScale * inventoryComponent->row * aspectRate;
	}

	point2D<int> InventorySystem::determineActualIntersectionSlot( components::transform* crosshairTransformComponent, components::transform* inventoryTransformComponent,
															  const float inventorySlotScale, const float inventorySlotHalfScale ) {
		float x_delta = crosshairTransformComponent->position[0] - inventoryTransformComponent->position[0] + inventorySlotHalfScale;
		float y_delta = crosshairTransformComponent->position[1] - inventoryTransformComponent->position[1] + inventorySlotHalfScale * aspectRate;
		
		return point2D<int>{ (int)(x_delta / inventorySlotScale), (int)(y_delta / (inventorySlotScale * aspectRate)) };
	}
} ///< namespace GLVM::ecs
