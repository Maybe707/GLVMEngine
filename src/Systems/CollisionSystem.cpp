// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// This file is part of Game Loop Versatile Modules (GLVM)
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Systems/CollisionSystem.hpp"
#include "Archetypes/ItemArchetype.hpp"
#include "Archetypes/LevelChunkArchetype.hpp"
#include "ComponentManager.hpp"
#include "Components/ActorComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ColliderFlagsComponent.hpp"
#include "Components/InventoryComponent.hpp"
#include "Components/InventorySlotComponent.hpp"
#include "Components/ItemComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Vector.hpp"
#include "VertexMath.hpp"
#include <climits>
#include "ArchetypeECS/ArchECS_Types.hpp"
#include "ArchetypeECS/ArchECS_Utils.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"
#include "Archetypes/EnemyArchetype.hpp"
#include "Components/ActorComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Texture.hpp"
#include "VertexMath.hpp"
#include <Systems/ProjectileSystem.hpp>
#include <cstdint>
#include <sys/types.h>
#include "ArchetypeECS/ArchECS_World.hpp"
#include "Archetypes/PlayerArchetype.hpp"
#include "Archetypes/ProjectileArchetype.hpp"
#include "Archetypes/StaticMeshArchetype.hpp"

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
		namespace arch = GLVM::ecs::arch;

//		arch::EntityLocation collidedEntityLocation = arch::world.archetypes
		cachedArchetypesNumber = 0;
		for( uint32_t i = 0; i < arch::world.archetypes.GetSize(); ++i ) {
			arch::Archetype* arch = arch::world.archetypes[i];
			arch::componentMask requiredMask = (1ul << arch::ComponentsIndices::COLLIDER_COMPONENT) |
				(1ul << arch::ComponentsIndices::COLLIDER_FLAGS_COMPONENT) |
				(1ul << arch::ComponentsIndices::TRANSFORM_COMPONENT) |
				(1ul << arch::ComponentsIndices::MESH_COMPONENT);

			if( (arch->mask & requiredMask) == requiredMask ) {
				cachedArchetypes[cachedArchetypesNumber] = arch;
				++cachedArchetypesNumber;
			}
		}
			
		const float cameraSpeed = 5.5f * fDelta_Time_;
		for( uint32_t x = 0; x < cachedArchetypesNumber; ++x ) {
			arch::Archetype* arch = cachedArchetypes[x];
			components::transform* backtrackingTransforms = nullptr;
			components::collider*  backtrackingColliders  = nullptr;
			components::colliderFlags* backtrackingColliderFlags = nullptr;
			components::mesh*      backtrackingMeshes     = nullptr;
			switch( arch->mask ) {
			case arch::playerComponentMask:
				backtrackingTransforms = static_cast<arch::PlayerArchetype*>( arch )->transforms;
				backtrackingColliders = static_cast<arch::PlayerArchetype*>( arch )->colliders;
				backtrackingColliderFlags = static_cast<arch::PlayerArchetype*>( arch )->colliderFlags;
				backtrackingMeshes = static_cast<arch::PlayerArchetype*>( arch )->meshes;
				break;
			case arch::enemyComponentMask:
				backtrackingTransforms = static_cast<arch::EnemyArchetype*>( arch )->transforms;
				backtrackingColliders = static_cast<arch::EnemyArchetype*>( arch )->colliders;
				backtrackingColliderFlags = static_cast<arch::EnemyArchetype*>( arch )->colliderFlags;
				backtrackingMeshes = static_cast<arch::EnemyArchetype*>( arch )->meshes;
				break;
			case arch::staticMeshComponentMask:
				backtrackingTransforms = static_cast<arch::StaticMeshArchetype*>( arch )->transforms;
				backtrackingColliders = static_cast<arch::StaticMeshArchetype*>( arch )->colliders;
				backtrackingColliderFlags = static_cast<arch::StaticMeshArchetype*>( arch )->colliderFlags;
				backtrackingMeshes = static_cast<arch::StaticMeshArchetype*>( arch )->meshes;
				break;
			case arch::levelChunkComponentMask:
				backtrackingTransforms = static_cast<arch::LevelChunkArchetype*>( arch )->transforms;
				backtrackingColliders = static_cast<arch::LevelChunkArchetype*>( arch )->colliders;
				backtrackingColliderFlags = static_cast<arch::LevelChunkArchetype*>( arch )->colliderFlags;
				backtrackingMeshes = static_cast<arch::LevelChunkArchetype*>( arch )->meshes;
				break;
			case arch::itemComponentMask:
				backtrackingTransforms = static_cast<arch::ItemArchetype*>( arch )->transforms;
				backtrackingColliders = static_cast<arch::ItemArchetype*>( arch )->colliders;
				backtrackingColliderFlags = static_cast<arch::ItemArchetype*>( arch )->colliderFlags;
				backtrackingMeshes = static_cast<arch::ItemArchetype*>( arch )->meshes;
				break;
			}
			
			for(unsigned int i = 0; i < arch->entityCount; ++i) {
				[[maybe_unused]] uint32_t backtrackingEntityID = arch->entities[i];
				
				uint8_t groudCollisionTurnOffMask = (1u << 0) | (0u << 1) | (1u << 2) | (1u << 3);
				if( backtrackingColliderFlags && backtrackingColliders &&
					backtrackingMeshes && backtrackingTransforms ) {
				
					backtrackingColliderFlags[i].flags = backtrackingColliderFlags[i].flags & groudCollisionTurnOffMask;
					backtrackingColliders[i].colliders.clear();
					components::mesh backtrackinEntityMesh = backtrackingMeshes[i];
					components::MeshHandle backtrackingEntityMeshHandle = backtrackinEntityMesh.handle;
//					[[maybe_unused]] float backtrackingGltfFlag = backtrackinEntityMesh.gltf;
				
					components::transform backtrackingTransformComponent = backtrackingTransforms[i];
					vec3 backtrackingTransform = backtrackingTransformComponent.position;
					[[maybe_unused]] float backtrackingScale = backtrackingTransformComponent.scale;

					arch::componentMask requiredMask = (1ul << arch::ComponentsIndices::MOVE_COMPONENT);
					if ( (arch->mask & requiredMask) == requiredMask  ) {
						components::move* backtrackingMove = nullptr;
						switch( arch->mask ) {
						case arch::playerComponentMask:
							backtrackingMove = static_cast<arch::PlayerArchetype*>( arch )->moves;
							break;
						case arch::enemyComponentMask:
							backtrackingMove = static_cast<arch::EnemyArchetype*>( arch )->moves;
							break;
						case arch::itemComponentMask:
							backtrackingMove = static_cast<arch::ItemArchetype*>( arch )->moves;
							break;
						}
					
						backtrackingTransform += Normalize(backtrackingMove->frameMovement) * cameraSpeed;
						backtrackingTransform += backtrackingMove->gravity;
					}

					for( uint32_t i1 = 0; i1 < cachedArchetypesNumber; ++i1 ) {
						// if ( i == i1 )
						// 	continue;

						arch::Archetype* comparedArch = cachedArchetypes[i1];
						components::transform* comparedTransforms = nullptr;
						components::mesh*      comparedMeshes     = nullptr;
						switch( comparedArch->mask ) {
						case arch::playerComponentMask:
							comparedTransforms = static_cast<arch::PlayerArchetype*>( comparedArch )->transforms;
							comparedMeshes = static_cast<arch::PlayerArchetype*>( comparedArch )->meshes;
							break;
						case arch::enemyComponentMask:
							comparedTransforms = static_cast<arch::EnemyArchetype*>( comparedArch )->transforms;
							comparedMeshes = static_cast<arch::EnemyArchetype*>( comparedArch )->meshes;
							break;
						case arch::staticMeshComponentMask:
							comparedTransforms = static_cast<arch::StaticMeshArchetype*>( comparedArch )->transforms;
							comparedMeshes = static_cast<arch::StaticMeshArchetype*>( comparedArch )->meshes;
							break;
						case arch::levelChunkComponentMask:
							comparedTransforms = static_cast<arch::LevelChunkArchetype*>( comparedArch )->transforms;
							comparedMeshes = static_cast<arch::LevelChunkArchetype*>( comparedArch )->meshes;
							break;
						case arch::itemComponentMask:
							comparedTransforms = static_cast<arch::ItemArchetype*>( comparedArch )->transforms;
							comparedMeshes = static_cast<arch::ItemArchetype*>( comparedArch )->meshes;
							break;
						}

						for(unsigned int j = 0; j < comparedArch->entityCount; ++j) {
							[[maybe_unused]] uint32_t comparedEntityID = comparedArch->entities[j];
							if( backtrackingEntityID == comparedEntityID ) {
								continue;
							}
							
							if( comparedMeshes && comparedTransforms ) {
							
								components::mesh comparedEntityMesh = comparedMeshes[j];
								components::MeshHandle comparedEntityMeshHandle = comparedEntityMesh.handle;
//								[[maybe_unused]] float comparedGltfFlag  = comparedEntityMesh.gltf;
						
								components::transform comparedTransformComponent = comparedTransforms[j];
								vec3  comparedTransform = comparedTransformComponent.position;
								[[maybe_unused]] float comparedScale     = comparedTransformComponent.scale;

								arch::componentMask requiredMask = (1ul << arch::ComponentsIndices::MOVE_COMPONENT);
								if ( (comparedArch->mask & requiredMask) == requiredMask  ) {
									components::move* comparedMove = nullptr;
									switch( comparedArch->mask ) {
									case arch::playerComponentMask:
										comparedMove = static_cast<arch::PlayerArchetype*>( comparedArch )->moves;
										break;
									case arch::enemyComponentMask:
										comparedMove = static_cast<arch::EnemyArchetype*>( comparedArch )->moves;
										break;
									case arch::itemComponentMask:
										comparedMove = static_cast<arch::ItemArchetype*>( comparedArch )->moves;
										break;
									}

									if( comparedMove ) {
										comparedTransform += Normalize(comparedMove->frameMovement) * cameraSpeed;
										comparedTransform += comparedMove->gravity;
									}
								}

								// if ( !backtrackingGltfFlag ) {
								// 	backtrackingScale /= 2;
								// }

								// if ( !comparedGltfFlag ) {
								// 	comparedScale /= 2;
								// }
				
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
									if( backtrackingEntityID == 0 && (comparedEntityID == 54 || comparedEntityID == 55 ||
																	  comparedEntityID == 56 || comparedEntityID == 57 || comparedEntityID
																	  == 58) ) {
										std::cout << "ground collisiton between player: " << backtrackingEntityID << " and item: " <<
											comparedEntityID << std::endl;
									}
									
									uint8_t groudCollisionTurnOffMask = (0u << 0) | (1u << 1) | (0u << 2) | (0u << 3);
									backtrackingColliderFlags[i].flags = backtrackingColliderFlags->flags | groudCollisionTurnOffMask;
									backtrackingColliders[i].colliders.Push(comparedArch->entities[j]);
							
									continue;
								}
                    
								if(boxColliderFlag) {
									if( backtrackingEntityID == 0 && (comparedEntityID == 54 || comparedEntityID == 55 ||
																	  comparedEntityID == 56 || comparedEntityID == 57 || comparedEntityID
																	  == 58) ) {
										std::cout << "wall collisiton between player: " << backtrackingEntityID << " and item: " <<
											comparedEntityID << std::endl;
									}
									
									uint8_t wallCollisionTurnOffMask = (1u << 0) | (0u << 1) | (0u << 2) | (0u << 3);
									backtrackingColliderFlags[i].flags = backtrackingColliderFlags->flags | wallCollisionTurnOffMask;
									backtrackingColliders[i].colliders.Push(comparedArch->entities[j]);
							
									continue;
								}
							}
						}
					}
				}
			}
		}
		cachedArchetypesNumber = 0;
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
