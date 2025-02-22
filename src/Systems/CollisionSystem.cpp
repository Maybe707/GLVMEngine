// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// This file is part of Game Loop Versatile Modules (GLVM)
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Systems/CollisionSystem.hpp"
#include "ComponentManager.hpp"
#include "Components/ActorComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ControllerComponent.hpp"
#include "Components/CrosshairComponent.hpp"
#include "Components/InventoryComponent.hpp"
#include "Components/InventorySlotComponent.hpp"
#include "Components/ItemComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
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
		                               float backtrackingScale, float comparedScale,
									   components::MeshHandle backtrackingMeshHandle, components::MeshHandle comparedMeshHandle)
	{
		core::MeshAxisMaxAbsoluteValues backtrackingMeshAxisMaxAbsoluteValues = allMeshMaxAbsoluteValues[backtrackingMeshHandle.id];
		core::MeshAxisMaxAbsoluteValues comparedMeshAxisMaxAbsoluteValues     = allMeshMaxAbsoluteValues[comparedMeshHandle.id];
		// if ( backtrackingMeshHandle.id == 6 || backtrackingMeshHandle.id == 7 ) {
		// 	std::cout << "collition system" << std::endl;
		// 	std::cout << "back mesh id: " << backtrackingMeshHandle.id << std::endl;
		// 	std::cout << "compated mesh id: " << comparedMeshHandle.id << std::endl;
		// 	std::cout << "size of all mesh container: " << allMeshMaxAbsoluteValues.GetSize() << std::endl;
		// 	std::cout << "half width: " << allMeshMaxAbsoluteValues[backtrackingMeshHandle.id].absolute_x << std::endl;
		// 	std::cout << "half height: " << allMeshMaxAbsoluteValues[backtrackingMeshHandle.id].absolute_y << std::endl;
		// 	std::cout << "half deep: " << allMeshMaxAbsoluteValues[backtrackingMeshHandle.id].absolute_z << std::endl;
		// 	sleep(1);
		// }

		//std::cout << "absolute x: " << backtrackingMeshAxisMaxAbsoluteValues.absolute_x << std::endl;
        if(backtrackingPosition[0] + backtrackingMeshAxisMaxAbsoluteValues.absolute_x * backtrackingScale  >
		   comparedPosition[0] - comparedMeshAxisMaxAbsoluteValues.absolute_x * comparedScale &&
           backtrackingPosition[0] - backtrackingMeshAxisMaxAbsoluteValues.absolute_x * backtrackingScale  <
		   comparedPosition[0] + comparedMeshAxisMaxAbsoluteValues.absolute_x * comparedScale &&
           backtrackingPosition[1] + backtrackingMeshAxisMaxAbsoluteValues.absolute_y * backtrackingScale  >
		   comparedPosition[1] - comparedMeshAxisMaxAbsoluteValues.absolute_y * comparedScale &&
           backtrackingPosition[1] - backtrackingMeshAxisMaxAbsoluteValues.absolute_y * backtrackingScale  <
		   comparedPosition[1] + comparedMeshAxisMaxAbsoluteValues.absolute_y * comparedScale &&
           backtrackingPosition[2] + backtrackingMeshAxisMaxAbsoluteValues.absolute_z * backtrackingScale  >
		   comparedPosition[2] - comparedMeshAxisMaxAbsoluteValues.absolute_z * comparedScale &&
           backtrackingPosition[2] - backtrackingMeshAxisMaxAbsoluteValues.absolute_z * backtrackingScale  <
		   comparedPosition[2] + comparedMeshAxisMaxAbsoluteValues.absolute_z * comparedScale) {
				return true;
		}
        
		return false;
	}

	bool CCollisionSystem::SquareCollider(vec3 backtrackingPosition, vec3 comparedPosition,
										  float backtrackingScale, float comparedScale_X, float comparedScale_Y,
										  components::MeshHandle backtrackingMeshHandle, components::MeshHandle comparedMeshHandle)
	{
		core::MeshAxisMaxAbsoluteValues backtrackingMeshAxisMaxAbsoluteValues = allMeshMaxAbsoluteValues[backtrackingMeshHandle.id];
		core::MeshAxisMaxAbsoluteValues comparedMeshAxisMaxAbsoluteValues     = allMeshMaxAbsoluteValues[comparedMeshHandle.id];
		
		[[maybe_unused]] float aspectRatio = 1920.0f / 1080.0f;
        if(backtrackingPosition[0] + backtrackingMeshAxisMaxAbsoluteValues.absolute_x * backtrackingScale >
		   comparedPosition[0] - comparedMeshAxisMaxAbsoluteValues.absolute_x * comparedScale_X &&
           backtrackingPosition[0] - backtrackingMeshAxisMaxAbsoluteValues.absolute_x * backtrackingScale <
		   comparedPosition[0] + comparedMeshAxisMaxAbsoluteValues.absolute_x * comparedScale_X &&
           backtrackingPosition[1] + backtrackingMeshAxisMaxAbsoluteValues.absolute_y * backtrackingScale * aspectRatio  >
		   comparedPosition[1] - comparedMeshAxisMaxAbsoluteValues.absolute_y * comparedScale_Y * aspectRatio &&
           backtrackingPosition[1] - backtrackingMeshAxisMaxAbsoluteValues.absolute_y * backtrackingScale * aspectRatio  <
		   comparedPosition[1] + comparedMeshAxisMaxAbsoluteValues.absolute_y * comparedScale_Y * aspectRatio) {
				return true;
		}
        
		return false;
	}

	bool CCollisionSystem::DotCollider(vec3 backtrackingPosition, vec3 comparedPosition, float comparedScale,
									   components::MeshHandle comparedMeshHandle)
	{
		core::MeshAxisMaxAbsoluteValues comparedMeshAxisMaxAbsoluteValues     = allMeshMaxAbsoluteValues[comparedMeshHandle.id];

		[[maybe_unused]] float aspectRatio = 1920.0f / 1080.0f;
        if(backtrackingPosition[0] > comparedPosition[0] - comparedMeshAxisMaxAbsoluteValues.absolute_x * comparedScale &&
           backtrackingPosition[0] < comparedPosition[0] + comparedMeshAxisMaxAbsoluteValues.absolute_x * comparedScale &&
           backtrackingPosition[1] > comparedPosition[1] - comparedMeshAxisMaxAbsoluteValues.absolute_y * comparedScale * aspectRatio &&
           backtrackingPosition[1] < comparedPosition[1] + comparedMeshAxisMaxAbsoluteValues.absolute_y * comparedScale * aspectRatio) {
				return true;
		}
        
		return false;
	}
	
    bool CCollisionSystem::UpperActorCheck(vec3 backtrackingPosition, vec3 comparedPosition,
										   float backtrackingScale, float comparedScale,
										   components::MeshHandle backtrackingMeshHandle, components::MeshHandle comparedMeshHandle) {
		core::MeshAxisMaxAbsoluteValues backtrackingMeshAxisMaxAbsoluteValues = allMeshMaxAbsoluteValues[backtrackingMeshHandle.id];
		core::MeshAxisMaxAbsoluteValues comparedMeshAxisMaxAbsoluteValues     = allMeshMaxAbsoluteValues[comparedMeshHandle.id];

        if((backtrackingPosition[1] - backtrackingMeshAxisMaxAbsoluteValues.absolute_y * backtrackingScale) + 1.7f >
		   (comparedPosition[1] + (comparedMeshAxisMaxAbsoluteValues.absolute_y * comparedScale))) {
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
																					  cm::actor,
																					  cm::mesh>();
		
        const float cameraSpeed = 5.5f * fDelta_Time_;            
		unsigned int linkedEntitiesVectorSize = linkedEntities.GetSize();
		for(unsigned int i = 0; i < linkedEntitiesVectorSize; ++i) {
			const unsigned int backtrackingEntityRefCollider = linkedEntities[i];
			componentManager->GetComponent<cm::collider>(backtrackingEntityRefCollider)->groundCollision = false;
			componentManager->GetComponent<cm::collider>(backtrackingEntityRefCollider)->colliders.clear();
			cm::MeshHandle backtrackingEntityMeshHandle = componentManager->GetComponent<cm::mesh>(backtrackingEntityRefCollider)->handle;

			cm::transform* backtrackingTransformComponent = componentManager->
				GetComponent<cm::transform>(backtrackingEntityRefCollider);
			vec3 backtrackingTransform = backtrackingTransformComponent->position;
			float backtrackingScale = backtrackingTransformComponent->scale;
			float backtrackingGltfFlag = backtrackingTransformComponent->gltf;

			if ( componentManager->isComponentExists<cm::move>(backtrackingEntityRefCollider) ) {
				cm::move* backtrackingMove = componentManager->
					GetComponent<cm::move>(backtrackingEntityRefCollider);
				backtrackingTransform += Normalize(backtrackingMove->frameMovement) * cameraSpeed;
				backtrackingTransform += backtrackingMove->gravity;
			}

			for(unsigned int j = 0; j < linkedEntitiesVectorSize; ++j) {
				if ( i == j )
					continue;
				
                const unsigned int comparedEntityRefCollider     = linkedEntities[j];

				cm::MeshHandle comparedEntityMeshHandle = componentManager->GetComponent<cm::mesh>(comparedEntityRefCollider)->handle;
				cm::transform* comparedTransformComponent = componentManager->
					GetComponent<cm::transform>(comparedEntityRefCollider);
			    vec3  comparedTransform = comparedTransformComponent->position;
				float comparedScale     = comparedTransformComponent->scale;
				float comparedGltfFlag  = comparedTransformComponent->gltf;

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
											  comparedScale,
											  backtrackingEntityMeshHandle,
											  comparedEntityMeshHandle);
				if ( boxColliderFlag ) {
					upperActorCheckFlag = UpperActorCheck(backtrackingTransform,
														  comparedTransform,
														  backtrackingScale,
														  comparedScale,
														  backtrackingEntityMeshHandle,
														  comparedEntityMeshHandle);
				}
				
				if(upperActorCheckFlag && boxColliderFlag) {
                    componentManager->GetComponent<cm::collider>(backtrackingEntityRefCollider)->groundCollision = true;
					componentManager->GetComponent<cm::collider>(backtrackingEntityRefCollider)->colliders.Push(comparedEntityRefCollider);
                    continue;
                }
                    
                if(boxColliderFlag) {
                    componentManager->GetComponent<cm::collider>(backtrackingEntityRefCollider)->wallCollision = true;
					componentManager->GetComponent<cm::collider>(backtrackingEntityRefCollider)->colliders.Push(comparedEntityRefCollider);
                    continue;
                }
			}
		}

		/// This code implements when inventory is open, player press left mouse button and he is not holding an item
		if ( isInventoryOpened && !*isItemDraged && isLeftMouseButtonPressed && *isLeftMouseButtonReleased ) {
			*isLeftMouseButtonReleased = false;
			core::vector<Entity> linkedItemEntities = componentManager->collectLinkedEntities<cm::item, cm::mesh, cm::material, cm::transform, cm::collider>();
			core::vector<Entity> linkedCrosshairEntities = componentManager->collectLinkedEntities<cm::crosshair, cm::transform>();
			const unsigned int crosshairEntity = linkedCrosshairEntities[0];              ///< Thats ok to give array '0' element in this case because we have only one crosshair
			const cm::transform* crosshairTransform = componentManager->GetComponent<cm::transform>(crosshairEntity);
			cm::MeshHandle crosshairMeshhandle = componentManager->GetComponent<cm::mesh>(crosshairEntity)->handle;
			vec3 crosshairPosition;
			float crosshairScale = 0;
			float crosshairGltfFlag = 0;
			if ( linkedCrosshairEntities.GetSize() > 0 ) {
				crosshairPosition = crosshairTransform->position;                
				crosshairScale = crosshairTransform->scale;
				crosshairGltfFlag = crosshairTransform->gltf;
			}

			for ( unsigned int i = 0; i < linkedItemEntities.GetSize(); ++i ) {
				unsigned int entityItemContaining = linkedItemEntities[i];
				const cm::item* itemComponent = componentManager->GetComponent<cm::item>(entityItemContaining);
				if ( itemComponent->occupiedSlots.GetSize() == 0 )                        ///< If occupiedSlots is 0 then this item not in inventory
					continue;

				const cm::transform* itemTransformComponent = componentManager->GetComponent<cm::transform>(entityItemContaining);
				cm::MeshHandle itemMeshHandle = componentManager->GetComponent<cm::mesh>(entityItemContaining)->handle;
				vec3  itemPosition;
				float itemScale_X  = 0;
				float itemScale_Y  = 0;
				float itemGltfFlag = 0;
				constexpr float collitionCorrectnessMultiplayer = 0.8;
				itemPosition = itemTransformComponent->position;
				itemScale_X  = itemTransformComponent->scale * itemComponent->itemSlotType.width *
					collitionCorrectnessMultiplayer;
				itemScale_Y  = itemTransformComponent->scale * itemComponent->itemSlotType.height *
					collitionCorrectnessMultiplayer;
				itemGltfFlag = itemTransformComponent->gltf;

				if ( !crosshairGltfFlag ) {
					crosshairScale /= 2;
				}

				if ( !itemGltfFlag ) {
					itemScale_X /= 2;
				}

				bool squareColliderFlag = false;
				squareColliderFlag = SquareCollider(crosshairPosition, itemPosition,
													crosshairScale, itemScale_X, itemScale_Y,
													crosshairMeshhandle, itemMeshHandle);
				if ( squareColliderFlag ) {
					componentManager->GetComponent<cm::collider>(entityItemContaining)->wallCollision = true;
					componentManager->GetComponent<cm::collider>(entityItemContaining)->colliders.Push(crosshairEntity);

					cm::item* collidedItemComponent = componentManager->GetComponent<cm::item>(entityItemContaining);
					for ( unsigned int j = 0; j < collidedItemComponent->occupiedSlots.GetSize(); ++j ) {
						unsigned int inventorySlotEntity = collidedItemComponent->occupiedSlots[j];
						cm::inventorySlot* inventorySlotComponent = componentManager->GetComponent<cm::inventorySlot>(inventorySlotEntity);
						inventorySlotComponent->itemEntity = UINT_MAX;
					}
					collidedItemComponent->occupiedSlots.clear();
				}
			}
		}

		/// This code implements highlightning on inventory slots and show player is it possible to drop item in inventory
		if ( isInventoryOpened && *isItemDraged ) {
			core::vector<Entity> linkedItemEntities = componentManager->collectLinkedEntities<cm::item, cm::mesh, cm::material, cm::transform, cm::collider>();
			core::vector<unsigned int> inventoryEntities = componentManager->collectLinkedEntities<cm::inventory>();
			cm::inventory* inventoryComponent = componentManager->GetComponent<cm::inventory>(inventoryEntities[0]);
			inventoryComponent->highlightedSlots.clear();
			inventoryComponent->isAvailableHighlightedSlots = false;                         ///< Turn off highlightning flag before set it to the right value

			for ( unsigned int i = 0; i < linkedItemEntities.GetSize(); ++i ) {
				unsigned int entityItemContaining = linkedItemEntities[i];
				cm::collider* itemCollider = componentManager->GetComponent<cm::collider>(entityItemContaining);

				if ( itemCollider->wallCollision ) {
					cm::transform* itemTransform = componentManager->GetComponent<cm::transform>(entityItemContaining);
					vec3 itemPosition = itemTransform->position;
					
					core::vector<Entity> linkedInventorySlotEntities = componentManager->collectLinkedEntities<cm::collider,
																											   cm::transform,
																											   cm::inventorySlot>();
					core::vector<unsigned int> collidedInventorySlotEntities;
					core::vector<vec3> collidedInventorySlotTransforms;
					
					for ( unsigned int j = 0; j < linkedInventorySlotEntities.GetSize(); ++j ) {
						unsigned int inventorySlotEntity      = linkedInventorySlotEntities[j];
						cm::transform* inventorySlotTransform = componentManager->GetComponent<cm::transform>(inventorySlotEntity);
						cm::MeshHandle slotMeshHandle = componentManager->GetComponent<cm::mesh>(inventorySlotEntity)->handle;
						vec3  inventorySlotPosition = inventorySlotTransform->position;
						float inventorySlotScale    = inventorySlotTransform->scale;
						bool  isInventorySlot_GLTF  = inventorySlotTransform->gltf;

						if ( !isInventorySlot_GLTF )
							inventorySlotScale /= 2;

						bool squareColliderFlag = false;
						inventorySlotPosition[2] = 0.0f;                                        ///< We dont need z-axis here because we test collision for x-y plane with item pivot
						squareColliderFlag = DotCollider(itemPosition, inventorySlotPosition,
														 inventorySlotScale, slotMeshHandle);
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

					bubbleSortVector(newColliderEntities);
					int stateSlotsAvailability = slotsAvailabilityState(newColliderEntities);

					/// If this condition equal true then this means that we got inventory slots highlighted with green color what meancs thar player can drop item into inventory
					if ( (stateSlotsAvailability == INT_MAX && itemComponent->itemSlotType.height * itemComponent->itemSlotType.width == newColliderEntities.GetSize()) ||
						 (stateSlotsAvailability >= 0 && itemComponent->itemSlotType.height * itemComponent->itemSlotType.width == newColliderEntities.GetSize()) ) {
						inventoryComponent->isAvailableHighlightedSlots = true;
					}

					for ( unsigned int v = 0; v < newColliderEntities.GetSize(); ++v )
						inventoryComponent->highlightedSlots.Push(newColliderEntities[v]);
				}
			}
		} else {
			core::vector<unsigned int> inventoryEntities = componentManager->collectLinkedEntities<cm::inventory>();
			cm::inventory* inventoryComponent = componentManager->GetComponent<cm::inventory>(inventoryEntities[0]);

			inventoryComponent->highlightedSlots.clear();
		}
		
		/// This code implements state when inventory is opened, player hold item and press left mouse button
		if ( isInventoryOpened && isLeftMouseButtonPressed && *isLeftMouseButtonReleased && *isItemDraged ) {
			const core::vector<Entity> linkedItemEntities = componentManager->collectLinkedEntities<cm::item, cm::mesh, cm::material, cm::transform, cm::collider>();
			for ( unsigned int i = 0; i < linkedItemEntities.GetSize(); ++i ) {
				const unsigned int entityItemContaining = linkedItemEntities[i];
				cm::collider* itemCollider = componentManager->GetComponent<cm::collider>(entityItemContaining);
				const core::vector<Entity> linkedCrosshairEntities = componentManager->collectLinkedEntities<cm::crosshair, cm::transform>();
				bool isCrosshairCollided = false;
				for ( unsigned int n = 0; n < itemCollider->colliders.GetSize(); ++n ) {
					if ( itemCollider->colliders[n] == linkedCrosshairEntities[0] ) {
						isCrosshairCollided = true;
						break;
					}
				}
				
				if ( itemCollider->wallCollision && isCrosshairCollided ) {
					cm::transform* itemTransform = componentManager->GetComponent<cm::transform>(entityItemContaining);
					const vec3 itemPosition = itemTransform->position;
					
					const core::vector<Entity> linkedInventorySlotEntities = componentManager->collectLinkedEntities<cm::collider,
																											   cm::transform,
																											   cm::inventorySlot>();
					core::vector<unsigned int> collidedInventorySlotEntities;
					core::vector<vec3> collidedInventorySlotTransforms;
					for ( unsigned int j = 0; j < linkedInventorySlotEntities.GetSize(); ++j ) {
						const unsigned int inventorySlotEntity      = linkedInventorySlotEntities[j];
						const cm::transform* inventorySlotTransform = componentManager->GetComponent<cm::transform>(inventorySlotEntity);
						cm::MeshHandle slotMeshHandle = componentManager->GetComponent<cm::mesh>(inventorySlotEntity)->handle;
						vec3  inventorySlotPosition = inventorySlotTransform->position;
						float inventorySlotScale    = inventorySlotTransform->scale;
						bool  isInventorySlot_GLTF  = inventorySlotTransform->gltf;

						if ( !isInventorySlot_GLTF ) {
							inventorySlotScale /= 2;
						}

						bool squareColliderFlag = false;
						inventorySlotPosition[2] = 0.0f;
						squareColliderFlag = DotCollider(itemPosition, inventorySlotPosition,
														 inventorySlotScale, slotMeshHandle);
						if ( squareColliderFlag ) {
							collidedInventorySlotEntities.Push(inventorySlotEntity);
							collidedInventorySlotTransforms.Push(inventorySlotPosition);
						} else {
							continue;
						}		
					}

					if ( collidedInventorySlotEntities.GetSize() == 0 ) {
						componentManager->GetComponent<cm::item>(entityItemContaining)->occupiedSlots.clear();
						componentManager->CreateComponent<cm::actor>(entityItemContaining);
						componentManager->CreateComponent<cm::rigidBody>(entityItemContaining);
						*componentManager->GetComponent<cm::rigidBody>(entityItemContaining) = { .fMass_ = 2.0f };
						core::vector<unsigned int> playerEntities = componentManager->collectLinkedEntities<cm::controller>();
						cm::transform* playerTransform = componentManager->GetComponent<cm::transform>(playerEntities[0]);
						itemTransform->position = playerTransform->position;
						vec3 normalizedForward = Normalize(playerTransform->forward);
						itemTransform->position[0] += normalizedForward[0] * 2.5f;
						itemTransform->position[2] += normalizedForward[2] * 2.5f;
						itemTransform->scale = 0.05f;
						*isItemDraged = false;
						*isLeftMouseButtonReleased = false;
						itemCollider->itemDrag = false;
						itemCollider->wallCollision = false;
					}
					
					cm::item* itemComponent = componentManager->GetComponent<cm::item>(entityItemContaining);
					core::vector<unsigned int> newColliderEntities = searchItemSlots(itemComponent->itemSlotType, itemPosition, collidedInventorySlotEntities, collidedInventorySlotTransforms);
					unsigned int slotsNumberForItem = itemComponent->itemSlotType.height * itemComponent->itemSlotType.width;

					if ( newColliderEntities.GetSize() != slotsNumberForItem )
						return;
					
					bubbleSortVector(newColliderEntities);
					int stateSlotsAvailability = slotsAvailabilityState(newColliderEntities);
					if ( stateSlotsAvailability == INT_MAX ) {                                                         ///< Just drop an item into inventory because all slots are available
						itemComponent->occupiedSlots.clear();
						
						for ( unsigned int x = 0; x < newColliderEntities.GetSize(); ++x ) {
							cm::inventorySlot* invetorySlot = componentManager->GetComponent<cm::inventorySlot>(newColliderEntities[x]);
							invetorySlot->itemEntity = entityItemContaining;
							itemComponent->occupiedSlots.Push(newColliderEntities[x]);
						}
						*isItemDraged = false;
						*isLeftMouseButtonReleased = false;
						itemCollider->itemDrag = false;
						itemCollider->wallCollision = false;

						return;
					} else if ( stateSlotsAvailability == -1 ) {                                                       ///< We cant drop item into inventory
						return;
					} else if ( stateSlotsAvailability >= 0 ) {                                                        ///< We can switch holding item with another one
						cm::item* collidedItemComponent = componentManager->GetComponent<cm::item>(stateSlotsAvailability);
						for ( unsigned int j = 0; j < collidedItemComponent->occupiedSlots.GetSize(); ++j ) {
							unsigned int inventorySlotEntity = collidedItemComponent->occupiedSlots[j];
							cm::inventorySlot* inventorySlotComponent = componentManager->GetComponent<cm::inventorySlot>(inventorySlotEntity);
							inventorySlotComponent->itemEntity = UINT_MAX;
						}
						cm::collider* collidedItemColliderComponent = componentManager->GetComponent<cm::collider>(stateSlotsAvailability);
						core::vector<Entity> linkedCrosshairEntities = componentManager->collectLinkedEntities<cm::crosshair, cm::transform>();
						collidedItemColliderComponent->wallCollision = true;
						collidedItemColliderComponent->colliders.Push(linkedCrosshairEntities[0]);

						itemComponent->occupiedSlots.clear();
						
						for ( unsigned int x = 0; x < newColliderEntities.GetSize(); ++x ) {
							cm::inventorySlot* invetorySlot = componentManager->GetComponent<cm::inventorySlot>(newColliderEntities[x]);
							invetorySlot->itemEntity = entityItemContaining;
							itemComponent->occupiedSlots.Push(newColliderEntities[x]);
						}

						*isLeftMouseButtonReleased = false;
						itemCollider->itemDrag = false;
						itemCollider->wallCollision = false;
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
		vec3 candidatePosition = candidateTransform->position;

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

				if ( (pivot_slot_row + result_row_offset) >= 0 && (pivot_slot_col + result_col_offset) >= 0 &&
					 (pivot_slot_row + result_row_offset) < 8 && (pivot_slot_col + result_col_offset) < 8 ) 
					newColliderEntities.Push(inventoryComponent->slots[pivot_slot_row + result_row_offset][pivot_slot_col + result_col_offset]);
			}
		
		return newColliderEntities;
	}
	
	int CCollisionSystem::slotsAvailabilityState(const core::vector<unsigned int>& slots_) {
		namespace cm = GLVM::ecs::components;
        ComponentManager* componentManager = ComponentManager::GetInstance();
		unsigned int resultAccumulator = 0;
		int fieldState = -2;                                   ///< -2 is a default status value
			
		for ( unsigned int i = 0; i < slots_.GetSize(); ++i ) {
			cm::inventorySlot* invetorySlot = componentManager->GetComponent<cm::inventorySlot>(slots_[i]);
			if ( invetorySlot == nullptr )
				continue;
			
			if ( invetorySlot->itemEntity == UINT_MAX ) {
				++resultAccumulator;
			} else {
				if ( fieldState < 0 ) {
					fieldState = invetorySlot->itemEntity;     ///< Write entity on first appearence
				} else if ( fieldState != static_cast<int>(invetorySlot->itemEntity) ) {
					return -1;                                 ///< If found another entity that means some of slots holds by two items
				}
			}
		}
		
		if ( resultAccumulator == slots_.GetSize() )
			return INT_MAX;
		else {
			return fieldState;
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
