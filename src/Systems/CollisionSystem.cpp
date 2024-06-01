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
				} else {
					componentManager->GetComponent<cm::collider>(entityItemContaining)->bWall_Collision_ = false;
					componentManager->GetComponent<cm::collider>(entityItemContaining)->colliders.clear();
				}
			}
		}

		if ( isInventoryOpened && isLeftMouseButtonPressed && *isLeftMouseButtonReleased && *isItemDraged ) {
			core::vector<Entity> linkedItemEntities = componentManager->collectLinkedEntities<cm::item, cm::mesh, cm::material, cm::transform, cm::collider>();

			for ( unsigned int i = 0; i < linkedItemEntities.GetSize(); ++i ) {
				unsigned int entityItemContaining = linkedItemEntities[i];
				cm::collider* itemCollider = componentManager->GetComponent<cm::collider>(entityItemContaining);
				componentManager->GetComponent<cm::collider>(entityItemContaining)->colliders.clear();
//				componentManager->GetComponent<cm::collider>(entityItemContaining)->bWall_Collision_ = false;

				if ( itemCollider->bWall_Collision_ ) {
//					itemCollider->bWall_Collision_ = false;
//					std::cout << "TEST" << std::endl;
					cm::transform* itemTransform = componentManager->GetComponent<cm::transform>(entityItemContaining);
					vec3 itemPosition = itemTransform->tPosition;
					float itemScale   = itemTransform->fScale;
					bool  isItem_GLTF = itemTransform->gltf;

					if ( !isItem_GLTF ) {
						itemScale /= 2;
					}
					
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
						squareColliderFlag = SquareCollider(itemPosition, inventorySlotPosition,
															itemScale, inventorySlotScale);
						if ( squareColliderFlag ) {
							std::cout << "collided entities " << inventorySlotEntity << std::endl;
							std::cout << "colliede entity position " << "x: " << inventorySlotPosition[0] << " y: " <<
								inventorySlotPosition[1] << " z: " << inventorySlotPosition[2] << std::endl;
							collidedInventorySlotEntities.Push(inventorySlotEntity);
							collidedInventorySlotTransforms.Push(inventorySlotPosition);
//							componentManager->GetComponent<cm::collider>(entityItemContaining)->bWall_Collision_ = true;
							componentManager->GetComponent<cm::collider>(entityItemContaining)->colliders.Push(entityItemContaining);
						} else {
							continue;
						}		
					}
					std::cout << "item position " << "x: " << itemPosition[0] << " y: " <<
								itemPosition[1] << " z: " << itemPosition[2] << std::endl;
					core::vector<float> distanceVectorsFromItemToInventorySlot;
					for ( unsigned int m = 0; m < collidedInventorySlotTransforms.GetSize(); ++m ) {
						vec3 collidedPosition = collidedInventorySlotTransforms[m];
						float aspectRatio = 1920.0f / 1080.0f;
						collidedPosition[0] = collidedPosition[0] * aspectRatio;
						vec3 localItemPosition = itemPosition;
						localItemPosition[0] = localItemPosition[0] * aspectRatio;
						distanceVectorsFromItemToInventorySlot.Push(VecLength(localItemPosition - collidedPosition));
						std::cout << "range " << distanceVectorsFromItemToInventorySlot[m] << std::endl;
						std::cout << "entity " << collidedInventorySlotEntities[m] << std::endl;
					}

					// Search 4 minimum distance from item to inventorySlot
					core::vector<unsigned int> newColliderEntities;
					unsigned int firstMinimumIndex = searchMinimumValueIndex(distanceVectorsFromItemToInventorySlot);
					if ( firstMinimumIndex != UINT_MAX ) {
						newColliderEntities.Push(collidedInventorySlotEntities[firstMinimumIndex]);
						collidedInventorySlotEntities.Remove(firstMinimumIndex);
						distanceVectorsFromItemToInventorySlot.Remove(firstMinimumIndex);
					}

					unsigned int secondMinimumIndex = searchMinimumValueIndex(distanceVectorsFromItemToInventorySlot);
					if ( secondMinimumIndex != UINT_MAX ) {
						newColliderEntities.Push(collidedInventorySlotEntities[secondMinimumIndex]);
						collidedInventorySlotEntities.Remove(secondMinimumIndex);
						distanceVectorsFromItemToInventorySlot.Remove(secondMinimumIndex);
					}

					unsigned int thirdMinimumIndex = searchMinimumValueIndex(distanceVectorsFromItemToInventorySlot);
					if ( thirdMinimumIndex != UINT_MAX ) {
						newColliderEntities.Push(collidedInventorySlotEntities[thirdMinimumIndex]);
						collidedInventorySlotEntities.Remove(thirdMinimumIndex);
						distanceVectorsFromItemToInventorySlot.Remove(thirdMinimumIndex);
					}

					unsigned int fourthMinimumIndex = searchMinimumValueIndex(distanceVectorsFromItemToInventorySlot);
					if ( fourthMinimumIndex != UINT_MAX ) {
						newColliderEntities.Push(collidedInventorySlotEntities[fourthMinimumIndex]);
						collidedInventorySlotEntities.Remove(fourthMinimumIndex);
						distanceVectorsFromItemToInventorySlot.Remove(fourthMinimumIndex);
					}

					cm::item* itemComponent = componentManager->GetComponent<cm::item>(entityItemContaining);
					bubbleSortVector(newColliderEntities);
					int stateSlotsAvailability = areSlotsAvailable(newColliderEntities);
//					std::cout << "state: " << stateSlotsAvailability << std::endl;
					if ( stateSlotsAvailability == INT_MAX ) {
						itemComponent->occupiedSlots.clear();
						
						for ( unsigned int x = 0; x < newColliderEntities.GetSize(); ++x ) {
//						std::cout << "new colliders entity: " << newColliderEntities[x] << std::endl;
							cm::inventorySlot* invetorySlot = componentManager->GetComponent<cm::inventorySlot>(newColliderEntities[x]);
							invetorySlot->itemEntity = entityItemContaining;
							std::cout << "new slot " << newColliderEntities[x] << std::endl;
							itemComponent->occupiedSlots.Push(newColliderEntities[x]);
						}

						// std::cout << "first vector " << VecLength(vec3(0.272436f - 0.2f, -0.670455 - (-0.8f), 0.0f)) << std::endl;
						// std::cout << "second vector " << VecLength(vec3(0.272436f - 0.4f, -0.670455 - (-0.63f), 0.0f)) << std::endl;
						
						// namespace cm = GLVM::ecs::components;
						// ComponentManager* componentManager = GLVM::ecs::ComponentManager::GetInstance();

						// core::vector<Entity> inventoryLinkedEntities = componentManager->collectLinkedEntities<cm::inventory>();
						// for ( unsigned int m = 0; m < inventoryLinkedEntities.GetSize(); ++m ) {
						// unsigned int inventoryEntity = inventoryLinkedEntities[m];
						// cm::inventory* inventoryComponent = componentManager->GetComponent<cm::inventory>(inventoryEntity);

						// std::cout << "TEST" << std::endl;
						// for ( unsigned int m = 0; m < 4; ++m )
						// 	for ( unsigned int n = 0; n < 8; ++n ) {
						// 		cm::inventorySlot* localItemSlot = componentManager->GetComponent<cm::inventorySlot>(inventoryComponent->slots[m][n]);
						// 		std::cout << "slot value: " << localItemSlot->itemEntity << std::endl;
						// 	}
						// }
						
//						std::cout << "ITEM DROPED" << std::endl;
						*isItemDraged = false;
						*isLeftMouseButtonReleased = false;
						itemCollider->itemDrag = false;
						itemCollider->bWall_Collision_ = false;

						return;
					} else if ( stateSlotsAvailability == -1 ) {
						// std::cout << "wall collision " << itemCollider->bWall_Collision_ << std::endl;
						// std::cout << "STILL HOLDING" << std::endl;
						return;
					} else if ( stateSlotsAvailability >= 0 ) {
						// TODO
					}
				}
			}
		}
	}

	int CCollisionSystem::areSlotsAvailable(const core::vector<unsigned int>& slots_) {
		namespace cm = GLVM::ecs::components;
        ComponentManager* componentManager = ComponentManager::GetInstance();
		
		int resultState = -1;
		unsigned int alreadyContainItemsAccumulator = 0;
		unsigned int allFreeStateAccumulator = 0;
		for ( unsigned int i = 0; i < slots_.GetSize(); ++i ) {
			cm::inventorySlot* invetorySlot = componentManager->GetComponent<cm::inventorySlot>(slots_[i]);
			if ( invetorySlot->itemEntity == UINT_MAX )
				++allFreeStateAccumulator;
			else if ( invetorySlot->itemEntity != UINT_MAX && alreadyContainItemsAccumulator == 0 ) {
				resultState = invetorySlot->itemEntity;
				++alreadyContainItemsAccumulator;
			} else if ( invetorySlot->itemEntity != UINT_MAX && alreadyContainItemsAccumulator > 0 ) {
				return -1;
			}
		}

		if ( allFreeStateAccumulator == 4 )
			return INT_MAX;
		else
			return resultState;
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
		if ( vector_.GetSize() == 0 )
			return;

		for ( unsigned int j = 0; j < vector_.GetSize(); ++j ) {
			for ( unsigned int i = 0; i < vector_.GetSize(); ++i ) {
				if ( vector_[i] > vector_[i + 1] ) {
					unsigned int temp = vector_[i];
					vector_[i] = vector_[i + 1];
					vector_[i + 1] = temp;
				}
			}
		}
	}
}
