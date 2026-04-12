#include "Systems/ItemSystem.hpp"
#include "ArchetypeECS/ArchECS_Types.hpp"
#include "Archetypes/ItemArchetype.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ColliderFlagsComponent.hpp"
#include "Components/CrosshairComponent.hpp"
#include "Components/InventoryComponent.hpp"
#include "Components/InventorySlotComponent.hpp"
#include "Components/ItemComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"
#include <cstdint>
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
		
		bool isSlotFound = false;
		unsigned int row = inventoryComponent->row;
		unsigned int col = inventoryComponent->col;

		arch::EntityLocation itemLocation = arch::world.entityLocations[arch::getId( itemEntity )];
		arch::ItemArchetype* itemArch = static_cast<arch::ItemArchetype*>(itemLocation.arch);
		const uint32_t itemIndex = itemLocation.index;
		cm::item* itemComponent = &itemArch->items[itemIndex];

		unsigned int item_width = itemComponent->itemSlotType.width;
		unsigned int item_height = itemComponent->itemSlotType.height;

		std::cout << "item width: " << item_width << std::endl;
		std::cout << "item height: " << item_height << std::endl;
		for ( unsigned int i = 0; i < row - item_height + 1; ++i )
			for ( unsigned int j = 0; j < col - item_width + 1; ++j ) {
				core::vector<unsigned int> maybeAvailabeSlots;
				core::vector<unsigned int> indicesOfMaybeAvailableSlots;
				for ( unsigned int m = i; m < i + item_height; ++m )
					for ( unsigned int n = j; n < j + item_width; ++n ) {
						maybeAvailabeSlots.Push(inventoryComponent->slots[m][n]);
						indicesOfMaybeAvailableSlots.Push(m * col + n);
					}

				unsigned int isAllSlotsAvailable = 0;
				for ( unsigned int v = 0; v < maybeAvailabeSlots.GetSize(); ++v ) {
					if ( maybeAvailabeSlots[v] == UINT_MAX ) {
//						std::cout << "inc" << std::endl;
						++isAllSlotsAvailable;
					} else {
//						std::cout << "dec" << std::endl;
						--isAllSlotsAvailable;
					}
				}
//				std::cout << "all slots: " << isAllSlotsAvailable << std::endl;
				if ( maybeAvailabeSlots.GetSize() == isAllSlotsAvailable ) {
					for ( unsigned int w = 0; w < maybeAvailabeSlots.GetSize(); ++w ) {
						unsigned int row_index = indicesOfMaybeAvailableSlots[w] / row;
						unsigned int col_index = indicesOfMaybeAvailableSlots[w] % col;
						inventoryComponent->slots[row_index][col_index] = itemEntity;
						itemComponent->occupiedSlots.Push(indicesOfMaybeAvailableSlots[w]);
					}
					
					isSlotFound = true;
					return isSlotFound;
				}
			}

		return isSlotFound;
	}
	
	void ItemSystem::Update() {
		namespace cm = GLVM::ecs::components;

		if (!isInventoryOpened) {
			for( uint32_t i = 0; i < arch::world.archetypes.GetSize(); ++i ) {
				arch::Archetype* arch = arch::world.archetypes[i];
				arch::componentMask requiredMask = (1ul << arch::ComponentsIndices::TRANSFORM_COMPONENT) |
					(1ul << arch::ComponentsIndices::INVENTORY_COMPONENT);

				if( (arch->mask & requiredMask) == requiredMask ) {
					cachedInventoryArchetype = arch;
				}
			}

			components::inventory*                inventory_inventoriesView   = nullptr;
			uint32_t inventoryEntityCount = 0;
			if( cachedInventoryArchetype != nullptr ) {
				inventoryEntityCount = cachedInventoryArchetype->entityCount;
				switch( cachedInventoryArchetype->mask ) {
				case arch::inventoryComponentMask:
					inventory_inventoriesView   = static_cast<arch::InventoryArchetype*>( cachedInventoryArchetype )->invetories;
					break;
				}
			}

			for( uint32_t i = 0; i < arch::world.archetypes.GetSize(); ++i ) {
				arch::Archetype* arch = arch::world.archetypes[i];
				arch::componentMask requiredMask = (1ul << arch::ComponentsIndices::TRANSFORM_COMPONENT) |
					(1ul << arch::ComponentsIndices::ITEM_COMPONENT) |
					(1ul << arch::ComponentsIndices::MESH_COMPONENT) |
					(1ul << arch::ComponentsIndices::MATERIAL_COMPONENT) |
					(1ul << arch::ComponentsIndices::COLLIDER_COMPONENT) |
					(1ul << arch::ComponentsIndices::COLLIDER_FLAGS_COMPONENT);

				if( (arch->mask & requiredMask) == requiredMask ) {
					cachedItemArchetype = arch;
				}
			}


			components::collider* itemColliderComponentView = nullptr;
			components::item*     itemsView                 = nullptr;                      
			uint32_t itemEntityCount = 0;
			if( cachedItemArchetype != nullptr ) {
				itemEntityCount = cachedItemArchetype->entityCount;
				switch( cachedItemArchetype->mask ) {
				case arch::itemComponentMask:
					itemColliderComponentView = static_cast<arch::ItemArchetype*>( cachedItemArchetype )->colliders;
					itemsView                 = static_cast<arch::ItemArchetype*>( cachedItemArchetype )->items;
					break;
				}
			}
			
			for ( unsigned int m = 0; m < inventoryEntityCount; ++m ) {
				cm::inventory* inventoryComponent = &inventory_inventoriesView[m];

				for ( unsigned int i = 0; i < itemEntityCount; ++i ) {
					unsigned int itemEntity = cachedItemArchetype->entities[i];
					cm::collider* itemColliderComponent = &itemColliderComponentView[i];

					for ( unsigned int j = 0; j < itemColliderComponent->colliders.GetSize(); ++j ) {
						if ( itemColliderComponent->colliders[j] == inventoryComponent->entityOwner &&
							itemsView[i].isActor ) {
							if ( putItem2x2(inventoryComponent, itemEntity) ) {
								itemsView[i].isActor = false;
//								componentManager->RemoveComponent<cm::actor>(itemEntity);
//								componentManager->RemoveComponent<cm::rigidBody>(itemEntity);
							} else {
								std::cout << "No suitable slots for that item in inventory" << std::endl;
							}
						}
					}
				}
			}
		}

		if(isInventoryOpened) {
			for( uint32_t i = 0; i < arch::world.archetypes.GetSize(); ++i ) {
				arch::Archetype* arch = arch::world.archetypes[i];
				arch::componentMask requiredMask = (1ul << arch::ComponentsIndices::TRANSFORM_COMPONENT) |
					(1ul << arch::ComponentsIndices::CROSSHAIR_TAG_COMPONENT);

				if( (arch->mask & requiredMask) == requiredMask ) {
					cachedCrosshairArchetype = arch;
				}
			}

			components::transform* crosshairTransformComponentView = nullptr;
			switch( cachedCrosshairArchetype->mask ) {
			case arch::crosshairComponentMask:
				crosshairTransformComponentView = static_cast<arch::CrosshairArchetype*>( cachedCrosshairArchetype )->transforms;
				break;
			}

			for( uint32_t i = 0; i < arch::world.archetypes.GetSize(); ++i ) {
				arch::Archetype* arch = arch::world.archetypes[i];
				arch::componentMask requiredMask = (1ul << arch::ComponentsIndices::TRANSFORM_COMPONENT) |
					(1ul << arch::ComponentsIndices::ITEM_COMPONENT) |
					(1ul << arch::ComponentsIndices::MESH_COMPONENT) |
					(1ul << arch::ComponentsIndices::MATERIAL_COMPONENT) |
					(1ul << arch::ComponentsIndices::COLLIDER_COMPONENT) |
					(1ul << arch::ComponentsIndices::COLLIDER_FLAGS_COMPONENT);

				if( (arch->mask & requiredMask) == requiredMask ) {
					cachedItemArchetype = arch;
				}
			}

			components::transform* itemTransformComponentView = nullptr;
			switch( cachedCrosshairArchetype->mask ) {
			case arch::crosshairComponentMask:
				itemTransformComponentView = static_cast<arch::CrosshairArchetype*>( cachedCrosshairArchetype )->transforms;
				break;
			}

			components::transform* crosshairTransformComponent = &crosshairTransformComponentView[0];
			for ( unsigned int i = 0; i < cachedItemArchetype->entityCount; ++i ) {
				int entityItemContaining = cachedItemArchetype->entities[i];
				cm::transform* itemTransformComponent = &itemTransformComponentView[i];

				// bool isCrosshairCollided = false;
				// for ( unsigned int n = 0; n < itemColliderComponent->colliders.GetSize(); ++n ) {
				// 	if ( itemColliderComponent->colliders[n] == linkedCrosshairEntities[0] ) {                ///< Is there a crosshair among the colliders
				// 		isCrosshairCollided = true;
				// 		break;
				// 	}
				// }
				
				// if ( itemColliderComponent->wallCollision && isCrosshairCollided ) {
				// 	itemTransformComponent->position = crosshairTransformComponent->position;                 ///< Set crosshair position to draged item
				// 	itemColliderComponent->itemDrag = true;
				// 	isItemDraged = true;
				// }
				if ( *dragedItemEntity >= 0 && *dragedItemEntity == entityItemContaining ) {
					itemTransformComponent->position = crosshairTransformComponent->position;                 ///< Set crosshair position to draged itemx
				}
			}
		}

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
