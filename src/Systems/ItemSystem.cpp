#include "Systems/ItemSystem.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/CrosshairComponent.hpp"
#include "Components/TransformComponent.hpp"

namespace GLVM::ecs
{
	void ItemSystem::Update() {
		namespace cm = GLVM::ecs::components;
        ComponentManager* componentManager = GLVM::ecs::ComponentManager::GetInstance();

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
					// std::cout << "size: " << itemColliderComponent->colliders.GetSize() << std::endl;
					// std::cout << "j: " << j << std::endl;

					// // for ( unsigned int n = 0; n < itemColliderComponent->colliders.GetSize(); ++n )
					// // 	std::cout << "entity: " << itemColliderComponent->colliders[n] << std::endl;

					// if ( inventoryComponent == nullptr ) {
					// 	std::cout << "nullptr " << std::endl;
					// 	continue;
					// }
					
					if ( itemColliderComponent->colliders[j] == inventoryComponent->entityOwner ) {
						unsigned int nextItemSlot = inventoryComponent->containedItems;
						if ( nextItemSlot < 64 ) {
							inventoryComponent->items[nextItemSlot] = itemEntity;
							++inventoryComponent->containedItems;
//						componentManager->RemoveComponent<cm::mesh>(itemEntity);
//						componentManager->RemoveComponent<cm::collider>(itemEntity);
//							componentManager->RemoveComponent<cm::transform>(itemEntity);
							componentManager->RemoveComponent<cm::actor>(itemEntity);
							componentManager->RemoveComponent<cm::rigidBody>(itemEntity);
						}
					}
				}
			}
		}

		if(isInventoryOpened) {
			core::vector<Entity> linkedInventoryEntities = componentManager->collectLinkedEntities<cm::inventory>();
			core::vector<Entity> linkedCrosshairEntities = componentManager->collectLinkedEntities<cm::crosshair, cm::transform>();

			cm::transform* crosshairTransformComponent = componentManager->GetComponent<cm::transform>(linkedCrosshairEntities[0]);
			
			for ( unsigned int i = 0; i < linkedInventoryEntities.GetSize(); ++i ) {
				unsigned int entityInventoryContaining = linkedInventoryEntities[i];
				cm::inventory* inventoryComponent = componentManager->GetComponent<cm::inventory>(entityInventoryContaining);

				for ( unsigned int j = 0; j < inventoryComponent->containedItems; ++j ) {
					unsigned int entityItemContaining = inventoryComponent->items[j];
					cm::collider* itemColliderComponent = componentManager->GetComponent<cm::collider>(entityItemContaining);
					cm::transform* itemTransformComponent = componentManager->GetComponent<cm::transform>(entityItemContaining);

					if ( *isLeftMouseButtonReleased && isItemDraged && isLeftMouseButtonPressed ) {
						isItemDraged = false;
						*isLeftMouseButtonReleased = false;
						itemColliderComponent->itemDrag = false;
						itemColliderComponent->bWall_Collision_ = false;
						std::cout << "first" << std::endl;
						return;
					}
					
					if ( itemColliderComponent->bWall_Collision_ ) {
//						std::cout << "item drag collisiton detected" << std::endl;
						itemTransformComponent->tPosition = crosshairTransformComponent->tPosition;
						itemColliderComponent->itemDrag = true;
						isItemDraged = true;
						std::cout << "second" << std::endl;
//						std::cout << "item position: " << itemTransformComponent->tPosition << std::endl;
					} 
				}
			}
		}
	}
} // namespace GLVM::ecs
