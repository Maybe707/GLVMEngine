// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// This file is part of Game Loop Versatile Modules (GLVM)
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Systems/CollisionSystem.hpp"
#include "ComponentManager.hpp"
#include "Components/ActorComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/CrosshairComponent.hpp"
#include "Components/InventoryComponent.hpp"
#include "Components/InventorySlotComponent.hpp"
#include "Components/ItemComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "EntityManager.hpp"
#include "Event.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "EventsStack.hpp"
#include "Vector.hpp"
#include "VertexMath.hpp"
#include <climits>

namespace GLVM::ecs
{
	bool CCollisionSystem::BoxCollider(vec3 backtrackingPosition, vec3 comparedPosition,
		                               float backtrackingScale, float comparedScale)
	{
        if(backtrackingPosition[0] + backtrackingScale  > comparedPosition[0] - comparedScale &&
           backtrackingPosition[0] - backtrackingScale  < comparedPosition[0] + comparedScale &&
           backtrackingPosition[1] + backtrackingScale  > comparedPosition[1] - comparedScale &&
           backtrackingPosition[1] - backtrackingScale  < comparedPosition[1] + comparedScale &&
           backtrackingPosition[2] + backtrackingScale  > comparedPosition[2] - comparedScale &&
           backtrackingPosition[2] - backtrackingScale  < comparedPosition[2] + comparedScale) {
				return true;
		}
        
		return false;
	}

	bool CCollisionSystem::SquareCollider(vec3 backtrackingPosition, vec3 comparedPosition,
		                               float backtrackingScale, float comparedScale)
	{
		[[maybe_unused]] float aspectRatio = 1920.0f / 1080.0f;
        if(backtrackingPosition[0] + backtrackingScale > comparedPosition[0] - comparedScale &&
           backtrackingPosition[0] - backtrackingScale < comparedPosition[0] + comparedScale &&
           backtrackingPosition[1] + backtrackingScale * aspectRatio  > comparedPosition[1] - comparedScale * aspectRatio &&
           backtrackingPosition[1] - backtrackingScale * aspectRatio  < comparedPosition[1] + comparedScale * aspectRatio) {
				return true;
		}
        
		return false;
	}

	bool CCollisionSystem::DotCollider(vec3 backtrackingPosition, vec3 comparedPosition, float comparedScale)
	{
		[[maybe_unused]] float aspectRatio = 1920.0f / 1080.0f;
        if(backtrackingPosition[0] > comparedPosition[0] - comparedScale &&
           backtrackingPosition[0] < comparedPosition[0] + comparedScale &&
           backtrackingPosition[1] > comparedPosition[1] - comparedScale * aspectRatio &&
           backtrackingPosition[1] < comparedPosition[1] + comparedScale * aspectRatio) {
				return true;
		}
        
		return false;
	}
	
    bool CCollisionSystem::UpperActorCheck(vec3 backtrackingPosition, vec3 comparedPosition,
										   float backtrackingScale, float comparedScale) {
        if((backtrackingPosition[1] - backtrackingScale) + 1.7f >
		   (comparedPosition[1] + (comparedScale))) {
            return true;
        }

        return false;
    }

	void CCollisionSystem::Update()
	{
		namespace cm = GLVM::ecs::components;
		
        ComponentManager* componentManager = ComponentManager::GetInstance();
		core::vector<Entity> linkedEntities = componentManager->collectLinkedEntities<cm::collider,
																					  cm::transform,
																					  cm::actor>();
		
        float cameraSpeed = 5.5f * fDelta_Time_;            
		unsigned int linkedEntitiesVectorSize = linkedEntities.GetSize();
		for(unsigned int i = 0; i < linkedEntitiesVectorSize; ++i) {
			unsigned int backtrackingEntityRefCollider = linkedEntities[i];  
			componentManager->GetComponent<cm::collider>(backtrackingEntityRefCollider)->bGround_Collision_ = false;
			componentManager->GetComponent<cm::collider>(backtrackingEntityRefCollider)->colliders.clear();
			for(unsigned int j = 0; j < linkedEntitiesVectorSize; ++j) {
				if ( i == j )
					continue;
				
                unsigned int comparedEntityRefCollider     = linkedEntities[j];
				
				vec3 backtrackingTransform = componentManager->
					GetComponent<cm::transform>(backtrackingEntityRefCollider)->tPosition;
				vec3 backtrackingTransformUpper = componentManager->
					GetComponent<cm::transform>(backtrackingEntityRefCollider)->tPosition;
				float backtrackingScale = componentManager->
					GetComponent<cm::transform>(backtrackingEntityRefCollider)->fScale;
				float backtrackingGltfFlag = componentManager->
					GetComponent<cm::transform>(backtrackingEntityRefCollider)->gltf;
			    vec3  comparedTransform     = componentManager->
					GetComponent<cm::transform>(comparedEntityRefCollider)->tPosition;
				vec3 comparedTransformUpper = componentManager->
					GetComponent<cm::transform>(comparedEntityRefCollider)->tPosition;
				float comparedScale     = componentManager->
					GetComponent<cm::transform>(comparedEntityRefCollider)->fScale;
				float comparedGltfFlag = componentManager->
					GetComponent<cm::transform>(comparedEntityRefCollider)->gltf;

				if ( componentManager->isComponentExists<cm::move>(backtrackingEntityRefCollider) ) {
					cm::move* backtrackingMove = componentManager->
						GetComponent<cm::move>(backtrackingEntityRefCollider);
					backtrackingTransform += Normalize(backtrackingMove->frameMovement) * cameraSpeed;
					backtrackingTransform += backtrackingMove->gravity;
				}

				if ( componentManager->isComponentExists<cm::move>(comparedEntityRefCollider) ) {
					cm::move* comparedMove = componentManager->
						GetComponent<cm::move>(comparedEntityRefCollider);
					comparedTransform += Normalize(comparedMove->frameMovement) * cameraSpeed;
					comparedTransform += comparedMove->gravity;
				}
				
				if ( !backtrackingGltfFlag ) {
					backtrackingScale /= 2;
				}

				if ( !comparedGltfFlag ) {
					comparedScale /= 2;
				}
				
				bool boxColliderFlag = false;
				bool upperActorCheckFlag = false;
                boxColliderFlag = BoxCollider(backtrackingTransform,
											  comparedTransform,
											  backtrackingScale,
											  comparedScale);
				if ( boxColliderFlag ) {
					upperActorCheckFlag = UpperActorCheck(backtrackingTransformUpper,
														  comparedTransformUpper,
														  backtrackingScale,
														  comparedScale);
				}
				
				if(upperActorCheckFlag && boxColliderFlag) {
                    componentManager->GetComponent<cm::collider>(backtrackingEntityRefCollider)->bGround_Collision_ = true;
					componentManager->GetComponent<cm::collider>(backtrackingEntityRefCollider)->colliders.Push(comparedEntityRefCollider);
                    continue;
                }
                    
                if(boxColliderFlag) {
                    componentManager->GetComponent<cm::collider>(backtrackingEntityRefCollider)->bWall_Collision_ = true;
					componentManager->GetComponent<cm::collider>(backtrackingEntityRefCollider)->colliders.Push(comparedEntityRefCollider);
                    continue;
                }
			}
		}

		// std::cout << "inventory opened: " << isInventoryOpened << std::endl;
		// std::cout << "item draged: " << isItemDraged << std::endl;
		// std::cout << "left mouse button pressed: " << isLeftMouseButtonPressed << std::endl;
		// std::cout << "left mouse button released: " << *isLeftMouseButtonReleased << std::endl;
		
		if ( isInventoryOpened && !*isItemDraged && isLeftMouseButtonPressed && *isLeftMouseButtonReleased ) {
//			std::cout << "COLLISION" << std::endl;
			*isLeftMouseButtonReleased = false;
			core::vector<Entity> linkedItemEntities = componentManager->collectLinkedEntities<cm::item, cm::mesh, cm::material, cm::transform, cm::collider>();
			core::vector<Entity> linkedCrosshairEntities = componentManager->collectLinkedEntities<cm::crosshair, cm::transform>();
			vec3 crosshairPosition;
			float crosshairScale = 0;
			float crosshairGltfFlag = 0;
			if ( linkedCrosshairEntities.GetSize() > 0 ) {
				crosshairPosition = componentManager->
					GetComponent<cm::transform>(linkedCrosshairEntities[0])->tPosition;                ///< Thants ok to give array '0' element in this case because we have only one crosshair
				crosshairScale = componentManager->
					GetComponent<cm::transform>(linkedCrosshairEntities[0])->fScale;
				crosshairGltfFlag = componentManager->
					GetComponent<cm::transform>(linkedCrosshairEntities[0])->gltf;
			}

			for ( unsigned int i = 0; i < linkedItemEntities.GetSize(); ++i ) {
				unsigned int entityItemContaining = linkedItemEntities[i];
				cm::item* itemComponent = componentManager->GetComponent<cm::item>(entityItemContaining);
				if ( itemComponent->occupiedSlots.GetSize() == 0 )
					continue;

				cm::transform* itemTransformComponent = componentManager->GetComponent<cm::transform>(entityItemContaining);
				vec3  itemPosition;
				float itemScale = 0;
				float itemGltfFlag = 0;
				if ( itemTransformComponent != nullptr ) {
					itemPosition = componentManager->
						GetComponent<cm::transform>(entityItemContaining)->tPosition;
					itemScale     = componentManager->
						GetComponent<cm::transform>(entityItemContaining)->fScale;
					itemGltfFlag = componentManager->
						GetComponent<cm::transform>(entityItemContaining)->gltf;
				}

				if ( !crosshairGltfFlag ) {
					crosshairScale /= 2;
				}

				if ( !itemGltfFlag ) {
					itemScale /= 2;
				}

				// std::cout << "crosshair scale: " << crosshairScale << std::endl;
				// std::cout << "item scale: " << itemScale << std::endl;
				
				bool squareColliderFlag = false;
				squareColliderFlag = SquareCollider(crosshairPosition, itemPosition,
													crosshairScale, itemScale);
				if ( squareColliderFlag ) {
					componentManager->GetComponent<cm::collider>(entityItemContaining)->bWall_Collision_ = true;
					componentManager->GetComponent<cm::collider>(entityItemContaining)->colliders.Push(entityItemContaining);

					cm::item* collidedItemComponent = componentManager->GetComponent<cm::item>(entityItemContaining);
					for ( unsigned int j = 0; j < collidedItemComponent->occupiedSlots.GetSize(); ++j ) {
						unsigned int inventorySlotEntity = collidedItemComponent->occupiedSlots[j];
						cm::inventorySlot* inventorySlotComponent = componentManager->GetComponent<cm::inventorySlot>(inventorySlotEntity);
						inventorySlotComponent->itemEntity = UINT_MAX;
					}
					collidedItemComponent->occupiedSlots.clear();
				} else {
					componentManager->GetComponent<cm::collider>(entityItemContaining)->bWall_Collision_ = false;
					componentManager->GetComponent<cm::collider>(entityItemContaining)->colliders.clear();
				}
			}
		}


		if ( isInventoryOpened && *isItemDraged ) {
			std::cout << "TEST" << std::endl;
			core::vector<Entity> linkedItemEntities = componentManager->collectLinkedEntities<cm::item, cm::mesh, cm::material, cm::transform, cm::collider>();
			for ( unsigned int i = 0; i < linkedItemEntities.GetSize(); ++i ) {
				unsigned int entityItemContaining = linkedItemEntities[i];
				cm::collider* itemCollider = componentManager->GetComponent<cm::collider>(entityItemContaining);
				componentManager->GetComponent<cm::collider>(entityItemContaining)->colliders.clear();

				if ( itemCollider->bWall_Collision_ ) {
					cm::transform* itemTransform = componentManager->GetComponent<cm::transform>(entityItemContaining);
					vec3 itemPosition = itemTransform->tPosition;
					// float itemScale   = itemTransform->fScale;
					// bool  isItem_GLTF = itemTransform->gltf;

					// if ( !isItem_GLTF ) {
					// 	itemScale /= 2;
					// }
					
					core::vector<Entity> linkedInventorySlotEntities = componentManager->collectLinkedEntities<cm::collider,
																											   cm::transform,
																											   cm::inventorySlot>();
					core::vector<unsigned int> collidedInventorySlotEntities;
					core::vector<vec3> collidedInventorySlotTransforms;
					for ( unsigned int j = 0; j < linkedInventorySlotEntities.GetSize(); ++j ) {
						unsigned int inventorySlotEntity      = linkedInventorySlotEntities[j];
						cm::transform* inventorySlotTransform = componentManager->GetComponent<cm::transform>(inventorySlotEntity);
						vec3  inventorySlotPosition = inventorySlotTransform->tPosition;
						float inventorySlotScale    = inventorySlotTransform->fScale;
						bool  isInventorySlot_GLTF  = inventorySlotTransform->gltf;

						if ( !isInventorySlot_GLTF ) {
							inventorySlotScale /= 2;
						}

						bool squareColliderFlag = false;
						inventorySlotPosition[2] = 0.0f;
						squareColliderFlag = DotCollider(itemPosition, inventorySlotPosition,
														 inventorySlotScale);
						if ( squareColliderFlag ) {
							collidedInventorySlotEntities.Push(inventorySlotEntity);
							collidedInventorySlotTransforms.Push(inventorySlotPosition);
							componentManager->GetComponent<cm::collider>(entityItemContaining)->colliders.Push(entityItemContaining);
						} else {
							continue;
						}		
					}

					cm::item* itemComponent = componentManager->GetComponent<cm::item>(entityItemContaining);
					core::vector<unsigned int> newColliderEntities = searchItemSlots(itemComponent->itemSlotType, itemPosition, collidedInventorySlotEntities, collidedInventorySlotTransforms);
					core::vector<unsigned int> inventoryEntities = componentManager->collectLinkedEntities<cm::inventory>();
					cm::inventory* inventoryComponent = componentManager->GetComponent<cm::inventory>(inventoryEntities[0]);

					bubbleSortVector(newColliderEntities);
					int stateSlotsAvailability = areSlotsAvailable(newColliderEntities);

					inventoryComponent->highlightedSlots.clear();
					if ( (stateSlotsAvailability == INT_MAX && itemComponent->itemSlotType.height * itemComponent->itemSlotType.width == newColliderEntities.GetSize()) ||
						 (stateSlotsAvailability >= 0 && itemComponent->itemSlotType.height * itemComponent->itemSlotType.width == newColliderEntities.GetSize()) )
						inventoryComponent->isAvailableHighlightedSlots = true;
					else
						inventoryComponent->isAvailableHighlightedSlots = false;
						
					for ( unsigned int v = 0; v < newColliderEntities.GetSize(); ++v )
						inventoryComponent->highlightedSlots.Push(newColliderEntities[v]);
				}
			}
		} else {
			core::vector<unsigned int> inventoryEntities = componentManager->collectLinkedEntities<cm::inventory>();
			cm::inventory* inventoryComponent = componentManager->GetComponent<cm::inventory>(inventoryEntities[0]);

			inventoryComponent->highlightedSlots.clear();
		}
		
		
		if ( isInventoryOpened && isLeftMouseButtonPressed && *isLeftMouseButtonReleased && *isItemDraged ) {
			core::vector<Entity> linkedItemEntities = componentManager->collectLinkedEntities<cm::item, cm::mesh, cm::material, cm::transform, cm::collider>();
			for ( unsigned int i = 0; i < linkedItemEntities.GetSize(); ++i ) {
				unsigned int entityItemContaining = linkedItemEntities[i];
				cm::collider* itemCollider = componentManager->GetComponent<cm::collider>(entityItemContaining);
				componentManager->GetComponent<cm::collider>(entityItemContaining)->colliders.clear();

				if ( itemCollider->bWall_Collision_ ) {
					cm::transform* itemTransform = componentManager->GetComponent<cm::transform>(entityItemContaining);
					vec3 itemPosition = itemTransform->tPosition;
					// float itemScale   = itemTransform->fScale;
					// bool  isItem_GLTF = itemTransform->gltf;

					// if ( !isItem_GLTF ) {
					// 	itemScale /= 2;
					// }
					
					core::vector<Entity> linkedInventorySlotEntities = componentManager->collectLinkedEntities<cm::collider,
																											   cm::transform,
																											   cm::inventorySlot>();
					core::vector<unsigned int> collidedInventorySlotEntities;
					core::vector<vec3> collidedInventorySlotTransforms;
					for ( unsigned int j = 0; j < linkedInventorySlotEntities.GetSize(); ++j ) {
						unsigned int inventorySlotEntity      = linkedInventorySlotEntities[j];
						cm::transform* inventorySlotTransform = componentManager->GetComponent<cm::transform>(inventorySlotEntity);
						vec3  inventorySlotPosition = inventorySlotTransform->tPosition;
						float inventorySlotScale    = inventorySlotTransform->fScale;
						bool  isInventorySlot_GLTF  = inventorySlotTransform->gltf;

						if ( !isInventorySlot_GLTF ) {
							inventorySlotScale /= 2;
						}

						bool squareColliderFlag = false;
						inventorySlotPosition[2] = 0.0f;
						squareColliderFlag = DotCollider(itemPosition, inventorySlotPosition,
														 inventorySlotScale);
						if ( squareColliderFlag ) {
							collidedInventorySlotEntities.Push(inventorySlotEntity);
							collidedInventorySlotTransforms.Push(inventorySlotPosition);
							componentManager->GetComponent<cm::collider>(entityItemContaining)->colliders.Push(entityItemContaining);
						} else {
							continue;
						}		
					}

					cm::item* itemComponent = componentManager->GetComponent<cm::item>(entityItemContaining);
					core::vector<unsigned int> newColliderEntities = searchItemSlots(itemComponent->itemSlotType, itemPosition, collidedInventorySlotEntities, collidedInventorySlotTransforms);
					unsigned int slotsNumberForItem = itemComponent->itemSlotType.height * itemComponent->itemSlotType.width;

					if ( newColliderEntities.GetSize() != slotsNumberForItem ) {
						
						return;
					}
					
					bubbleSortVector(newColliderEntities);
					int stateSlotsAvailability = areSlotsAvailable(newColliderEntities);
					if ( stateSlotsAvailability == INT_MAX ) {
						itemComponent->occupiedSlots.clear();
						
						for ( unsigned int x = 0; x < newColliderEntities.GetSize(); ++x ) {
							cm::inventorySlot* invetorySlot = componentManager->GetComponent<cm::inventorySlot>(newColliderEntities[x]);
							invetorySlot->itemEntity = entityItemContaining;
							itemComponent->occupiedSlots.Push(newColliderEntities[x]);
						}
						*isItemDraged = false;
						*isLeftMouseButtonReleased = false;
						itemCollider->itemDrag = false;
						itemCollider->bWall_Collision_ = false;

						return;
					} else if ( stateSlotsAvailability == -1 ) {
						return;
					} else if ( stateSlotsAvailability >= 0 ) {
						cm::item* collidedItemComponent = componentManager->GetComponent<cm::item>(stateSlotsAvailability);
						for ( unsigned int j = 0; j < collidedItemComponent->occupiedSlots.GetSize(); ++j ) {
							unsigned int inventorySlotEntity = collidedItemComponent->occupiedSlots[j];
							cm::inventorySlot* inventorySlotComponent = componentManager->GetComponent<cm::inventorySlot>(inventorySlotEntity);
							inventorySlotComponent->itemEntity = UINT_MAX;
						}
						cm::collider* collidedItemColliderComponent = componentManager->GetComponent<cm::collider>(stateSlotsAvailability);
						collidedItemColliderComponent->bWall_Collision_ = true;
						collidedItemColliderComponent->colliders.clear();
						collidedItemColliderComponent->colliders.Push(entityItemContaining);

						itemComponent->occupiedSlots.clear();
						
						for ( unsigned int x = 0; x < newColliderEntities.GetSize(); ++x ) {
							cm::inventorySlot* invetorySlot = componentManager->GetComponent<cm::inventorySlot>(newColliderEntities[x]);
							invetorySlot->itemEntity = entityItemContaining;
							itemComponent->occupiedSlots.Push(newColliderEntities[x]);
						}

						*isLeftMouseButtonReleased = false;
						itemCollider->itemDrag = false;
						itemCollider->bWall_Collision_ = false;
						return;
					}
				}
			}
		}
	}

	core::vector<unsigned int> CCollisionSystem::searchItemSlots(components::ItemSlotType itemSlotType, vec3 itemPosition, const core::vector<unsigned int>& collidedInventorySlotEntities,
																	[[maybe_unused]] const core::vector<vec3>& collidedInventorySlotTransforms) {
		float aspectRatio = 1920.0f / 1080.0f;
		vec3 localItemPosition = itemPosition;
		localItemPosition[0] = localItemPosition[0] * aspectRatio;

		core::vector<unsigned int> newColliderEntities;
		namespace cm = GLVM::ecs::components;
		
        ComponentManager* componentManager = ComponentManager::GetInstance();
		unsigned int pivotEntity = 0;
		if ( collidedInventorySlotEntities.GetSize() > 0 )
			pivotEntity = collidedInventorySlotEntities[0];
		else
			return newColliderEntities;
		
		cm::transform* candidateTransform = componentManager->GetComponent<cm::transform>(pivotEntity);
		core::vector<unsigned int> inventoryEntity = componentManager->collectLinkedEntities<cm::inventory>();
		cm::inventory* inventoryComponent = componentManager->GetComponent<cm::inventory>(inventoryEntity[0]);
		int pivot_slot_row = -1;
		int pivot_slot_col = -1;
		for ( unsigned int i = 0; i < inventoryComponent->row; ++i )
			for ( unsigned int j = 0; j < inventoryComponent->col; ++j ) {
				if ( pivotEntity == inventoryComponent->slots[i][j] ) {
					pivot_slot_row = i;
					pivot_slot_col = j;
				}
			}
		vec3 candidatePosition = candidateTransform->tPosition;

		int row_offset = 0;
		int col_offset = 0;
		const unsigned int half_width = static_cast<int>(std::floor(itemSlotType.width / 2.0f));
		unsigned int even_odd_width_flag = 1;
		// if ( half_width == itemSlotType.width / 2.0f )
		// 	even_odd_width_flag = 0;
			
		// if ( itemPosition[0] >= candidatePosition[0] ) {
		// 	if ( pivot_slot_col < inventoryComponent->col - half_width && pivot_slot_col >= half_width ) {
		// 		col_offset = half_width;
		// 	} else if ( pivot_slot_col < half_width ) {
		// 		col_offset = half_width + (half_width - pivot_slot_col -1 + even_odd_width_flag);
		// 	} else if ( pivot_slot_col >= inventoryComponent->col - half_width ) {
		// 		col_offset = -half_width - (half_width - (inventoryComponent->col - pivot_slot_col)) - even_odd_width_flag;
		// 	}

		// } else {
		// 	if ( pivot_slot_col < inventoryComponent->col - half_width && pivot_slot_col >= half_width ) {
		// 		col_offset = -half_width;
		// 	} else if ( pivot_slot_col < half_width ) {
		// 		col_offset = half_width + (half_width - pivot_slot_col -1 + even_odd_width_flag);
		// 	} else if ( pivot_slot_col >= inventoryComponent->col - half_width ) {
		// 		col_offset = -half_width - (half_width - (inventoryComponent->col - pivot_slot_col)) - even_odd_width_flag;
		// 	}
		// }

		// const unsigned int half_height = static_cast<int>(std::floor(itemSlotType.height / 2.0f));
		// unsigned int even_odd_height_flag = 1;
		// if ( half_height == itemSlotType.height / 2.0f )
		// 	even_odd_height_flag = 0;

		// if ( itemPosition[1] >= candidatePosition[1] ) {
		// 	if ( pivot_slot_row < inventoryComponent->row - half_height && pivot_slot_row >= half_height ) {
		// 		row_offset = half_height;
		// 	} else if ( pivot_slot_row < half_height ) {
		// 		row_offset = half_height + (half_height - pivot_slot_row -1 + even_odd_height_flag);
		// 	} else if ( pivot_slot_row >= inventoryComponent->row - half_height ) {
		// 		row_offset = -(half_height) - (half_height - (inventoryComponent->row - pivot_slot_row)) - even_odd_height_flag;
		// 	}
		// } else {
		// 	if ( pivot_slot_row < inventoryComponent->row - half_height && pivot_slot_row >= half_height ) {
		// 		row_offset = -(half_height);
		// 	} else if ( pivot_slot_row < half_height ) {
		// 		row_offset = half_height + (half_height - pivot_slot_row -1 + even_odd_height_flag);
		// 	} else if ( pivot_slot_row >= inventoryComponent->row - half_height ) {
		// 		row_offset = -(half_height) - (half_height - (inventoryComponent->row - pivot_slot_row)) - even_odd_height_flag;
		// 	}
		// }

		if ( half_width == itemSlotType.width / 2.0f )
			even_odd_width_flag = 0;

		if ( !even_odd_width_flag ) {
			if ( itemPosition[0] >= candidatePosition[0] ) {
				col_offset = half_width;
			} else {
				col_offset = -half_width;
			}
		} else {
			col_offset = half_width;
		}

		const unsigned int half_height = static_cast<int>(std::floor(itemSlotType.height / 2.0f));
		unsigned int even_odd_height_flag = 1;
		if ( half_height == itemSlotType.height / 2.0f )
			even_odd_height_flag = 0;

		if ( !even_odd_height_flag ) {
			if ( itemPosition[1] >= candidatePosition[1] ) {
				row_offset = half_height;
			}  else {
				row_offset = -half_height;
			}
		} else {
			row_offset = half_height;
		}
		
		for ( unsigned int i = 0; i < itemSlotType.height; ++i )
			for ( unsigned int j = 0; j < itemSlotType.width; ++j ) {
				int result_row_offset = 0;
				int result_col_offset = 0;
				if ( row_offset < 0 )
					result_row_offset = row_offset + i;
				else if (row_offset > 0 )
					result_row_offset = row_offset - i;

				if ( col_offset < 0 )
					result_col_offset = col_offset + j;
				else if (col_offset > 0 )
					result_col_offset = col_offset - j;

				// std::cout << "x offset: " << pivot_slot_col + result_col_offset << std::endl;
				// std::cout << "y offset: " << pivot_slot_row + result_row_offset << std::endl;

				// bool existSlot = false;
				// for ( unsigned int n = 0; n < newColliderEntities.GetSize(); ++n ) {
				// 	if ( newColliderEntities[n] == inventoryComponent->slots[pivot_slot_row + result_row_offset][pivot_slot_col + result_col_offset] ) {
				// 		existSlot = true;
				// 		break;
				// 	}
				// }

				if ( (pivot_slot_row + result_row_offset) >= 0 && (pivot_slot_col + result_col_offset) >= 0 &&
					 (pivot_slot_row + result_row_offset) < 8 && (pivot_slot_col + result_col_offset) < 8 ) 
					newColliderEntities.Push(inventoryComponent->slots[pivot_slot_row + result_row_offset][pivot_slot_col + result_col_offset]);
			}

//		std::cout << "number of available slots: " << newColliderEntities.GetSize() << std::endl;
		
		return newColliderEntities;
	}
	
	int CCollisionSystem::areSlotsAvailable(const core::vector<unsigned int>& slots_) {
		namespace cm = GLVM::ecs::components;
        ComponentManager* componentManager = ComponentManager::GetInstance();
		
		int resultState = -1;
		cm::inventorySlot* inventorySlot = nullptr;
		if ( slots_.GetSize() > 0 )
			inventorySlot = componentManager->GetComponent<cm::inventorySlot>(slots_[0]);
		else
			resultState = -1;

		unsigned int alreadyContainItemsAccumulator = 0;
		if ( inventorySlot != nullptr )
			alreadyContainItemsAccumulator = inventorySlot->itemEntity;

		unsigned int allFreeStateAccumulator = 0;
		if ( alreadyContainItemsAccumulator == UINT_MAX )
			++allFreeStateAccumulator;
			
		for ( unsigned int i = 1; i < slots_.GetSize(); ++i ) {
			cm::inventorySlot* invetorySlot = componentManager->GetComponent<cm::inventorySlot>(slots_[i]);

			if ( alreadyContainItemsAccumulator == UINT_MAX && invetorySlot->itemEntity != UINT_MAX ) {
				alreadyContainItemsAccumulator = invetorySlot->itemEntity;
			} else if ( invetorySlot->itemEntity == UINT_MAX ) {
				++allFreeStateAccumulator;
			} else if ( invetorySlot->itemEntity == alreadyContainItemsAccumulator ) {
				alreadyContainItemsAccumulator = invetorySlot->itemEntity;
			} else if ( invetorySlot->itemEntity != UINT_MAX &&
						invetorySlot->itemEntity > 0 &&
						invetorySlot->itemEntity != alreadyContainItemsAccumulator ) {
				return resultState;
			}
		}
		
		if ( allFreeStateAccumulator == slots_.GetSize() )
			return INT_MAX;
		else {
			return alreadyContainItemsAccumulator;
		}
	}
	
	unsigned int CCollisionSystem::searchMinimumValueIndex(core::vector<float> vector_) {
		unsigned int indexAccumulator = 0;
		float valueAccumulator = 0;
		if ( vector_.GetSize() > 0 )
			valueAccumulator = vector_[0];
		else
			return UINT_MAX;
		
		for ( unsigned int i = 1; i < vector_.GetSize(); ++i ) {
			if ( vector_[i] < valueAccumulator ) {
				valueAccumulator = vector_[i];
				indexAccumulator = i;
			}
		}
		std::cout << "minumem " <<valueAccumulator << std::endl;
		return indexAccumulator;
	}

	void CCollisionSystem::bubbleSortVector(core::vector<unsigned int>& vector_) {
		unsigned int max = 0;
		if ( vector_.GetSize() > 0 )
			max = vector_.GetSize() - 1;
		else
			return;
		
		for ( unsigned int j = 0; j < max; ++j ) {
			for ( unsigned int i = 0; i < max; ++i ) {
				if ( vector_[i] > vector_[i + 1] ) {
					unsigned int temp = vector_[i];
					vector_[i] = vector_[i + 1];
					vector_[i + 1] = temp;
				}
			}
		}
	}
}
