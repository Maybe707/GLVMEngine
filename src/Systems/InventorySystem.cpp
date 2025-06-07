#include "Systems/InventorySystem.hpp"
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

			if ( *isItemDraged < 0 && isLeftMouseButtonPressed && *isLeftMouseButtonReleased ) {
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
							*isItemDraged = entity;
						}
					}
					
					for( unsigned int i = 0; i < 8; ++i ) {
						for( unsigned int j = 0; j < 8; ++j ) {
							std::cout << inventoryComponent->slots[i][j] << " ";
						}
						std::cout << std::endl;
					}
				}
				*isLeftMouseButtonReleased = false;
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
						const int itemWidth  = itemComponent->itemSlotType.width;
						const int itemHeight = itemComponent->itemSlotType.height;

						const int row    = intersectionSlot.y;
						const int column = intersectionSlot.x;

						/// Find left-upper pivot slot inventory
						int rowBasicOffset    = 0;
						int columnBasicOffset = 0;

						columnBasicOffset = calculateBasicOffset( itemWidth, inventoryTransformComponent->position[0],
																  crosshairTransformComponent->position[0], column, inventorySlotScale );
						rowBasicOffset    = calculateBasicOffset( itemHeight, inventoryTransformComponent->position[1],
																  crosshairTransformComponent->position[1], row, inventorySlotScale * aspectRate );

						int pivotRow    = row - rowBasicOffset;
						int pivotColumn = column - columnBasicOffset;

						clamp<int>( 0, pivotRow, static_cast<int>(inventoryComponent->row) - itemHeight );
						clamp<int>( 0, pivotColumn, static_cast<int>(inventoryComponent->col) - itemWidth );

						core::vector<unsigned int> potentialOccupiedSlots;
						int isSwapable = determineSwappableField( itemComponent, itemWidth, itemHeight, pivotRow, pivotColumn, inventoryComponent, potentialOccupiedSlots );
						
						if( isSwapable == -1 ) {
							itemComponent->occupiedSlots = potentialOccupiedSlots;
							fillInventorySlots( itemComponent, itemWidth, itemHeight, inventoryComponent, *isItemDraged );
						} else if ( isSwapable > 0 ) {
							cm::item* swapedItemComponent = componentManager->GetComponent<cm::item>( isSwapable );
							itemComponent->occupiedSlots = potentialOccupiedSlots;
							fillInventorySlots( swapedItemComponent, swapedItemComponent->itemSlotType.width, swapedItemComponent->itemSlotType.height,
												inventoryComponent, UINT_MAX );
							
							swapedItemComponent->occupiedSlots.clear();
							fillInventorySlots( itemComponent, itemWidth, itemHeight, inventoryComponent, *isItemDraged );
							
							*isLeftMouseButtonReleased = false;
							*isItemDraged = isSwapable;
							return;
						}
					}
				}

				*isLeftMouseButtonReleased = false;
				*isItemDraged = -1;
			}
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
