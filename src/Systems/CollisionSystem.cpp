// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Systems/CollisionSystem.hpp"
#include "ComponentManager.hpp"
#include "Components/ActorComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/CrosshairComponent.hpp"
#include "Components/InventoryComponent.hpp"
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

		if ( isInventoryOpened && !isItemDraged && isLeftMouseButtonPressed && *isLeftMouseButtonReleased ) {
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

				std::cout << "crosshair scale: " << crosshairScale << std::endl;
				std::cout << "item scale: " << itemScale << std::endl;
				
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
	}

}
