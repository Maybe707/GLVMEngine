#include "Systems/CollisionSystem.hpp"
#include "ComponentManager.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "EntityManager.hpp"
#include "Event.hpp"
#include "Components/EventComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Stack.hpp"
#include "Vector.hpp"
#include "VertexMath.hpp"

namespace GLVM::ecs
{
	bool CCollisionSystem::BoxCollider(vec3 backtrackingPosition, vec3 comparedPosition,
		                               float backtrackingScale, float comparedScale)
	{
        if(backtrackingPosition[0] + backtrackingScale / 2 > comparedPosition[0] - comparedScale / 2 &&
           backtrackingPosition[0] - backtrackingScale / 2 < comparedPosition[0] + comparedScale / 2 &&
           backtrackingPosition[1] + backtrackingScale / 2 > comparedPosition[1] - comparedScale / 2 &&
           backtrackingPosition[1] - backtrackingScale / 2 < comparedPosition[1] + comparedScale / 2 &&
           backtrackingPosition[2] + backtrackingScale / 2 > comparedPosition[2] - comparedScale / 2 &&
           backtrackingPosition[2] - backtrackingScale / 2 < comparedPosition[2] + comparedScale / 2) {
				return true;
		}
        
		return false;
	}

    bool CCollisionSystem::UpperActorCheck(vec3 backtrackingPosition, vec3 comparedPosition,
										   float backtrackingScale, float comparedScale) {
        if((backtrackingPosition[1] - backtrackingScale / 2) >
		   (comparedPosition[1] + (comparedScale / 2))) {
            return true;
        }

        return false;
    }

	// bool CCollisionSystem::RayCast(vec3 rayCasterPosition, vec3 receiverPosition,
	// 							   float rayCasterScale, float receiverScale) {
	// 	float yaw, pitch;
	// 	const float kSensitivity = 0.1;
    //     yaw = g_eEvent.mousePointerPosition.offset_X;
    //     pitch = g_eEvent.mousePointerPosition.offset_Y;
    //     yaw *= kSensitivity;
    //     pitch *= kSensitivity;

    //     g_eEvent.mousePointerPosition.pitch = pitch;
    //     g_eEvent.mousePointerPosition.yaw = yaw;
        
    //     if(pitch > 89.0f)
    //         pitch = 89.0f;
    //     if(pitch < -89.0f)
    //         pitch = -89.0f;
	// 	vec3 forward;
    //     forward[0] = std::cos(Radians(yaw)) * std::cos(Radians(pitch));
    //     forward[1] = std::sin(Radians(pitch));
    //     forward[2] = std::sin(Radians(yaw)) * std::cos(Radians(pitch));
    //     forward = Normalize(forward);

	// 	// vec3 vectorXZ = vec3{ rayCasterPosition[0], 0.0f, rayCasterPosition[2] } -
	// 	// 				vec3{ receiverPosition[0], 0.0f, receiverPosition[2] };

	// 	// vectorXZ -= receiverScale / 2;
	// 	// float distance = std::sqrt(vectorXZ[0] * vectorXZ[0] + vectorXZ[1] * vectorXZ[1] + vectorXZ[2] *\ vectorXZ[2]);
	// 	// forward *= distance;
	// 	forward *= 5.0f;
		
	// 	if ( forward[0] 
	// }
	
	void CCollisionSystem::Update()
	{
		namespace cm = GLVM::ecs::components;
		
        ComponentManager* componentManager = ComponentManager::GetInstance();
		core::vector<Entity> linkedEntities = componentManager->collectLinkedEntities<cm::collider,
																					  cm::transform>();
		
		core::vector<Entity> linkedEntitiesWithMove = componentManager->collectLinkedEntities<cm::collider,
																					  cm::transform,
																					  cm::move>();
		
        float cameraSpeed = 5.5f * fDelta_Time_;            
		unsigned int linkedEntitiesVectorSize = linkedEntities.GetSize();
		unsigned int linkedEntitiesVectorSizeWithMove = linkedEntitiesWithMove.GetSize();
		for(unsigned int i = 0; i < linkedEntitiesVectorSize; ++i) {
			for(unsigned int j = 0; j < linkedEntitiesVectorSize; ++j) {
				if ( i == j )
					continue;
				
                unsigned int backtrackingEntityRefCollider = linkedEntities[i];  
                unsigned int comparedEntityRefCollider     = linkedEntities[j];
				
				vec3 backtrackingTransform = componentManager->
					GetComponent<cm::transform>(backtrackingEntityRefCollider)->tPosition;
				vec3 backtrackingTransformUpper = componentManager->
					GetComponent<cm::transform>(backtrackingEntityRefCollider)->tPosition;
				float backtrackingScale = componentManager->
					GetComponent<cm::transform>(backtrackingEntityRefCollider)->fScale;
			    vec3  comparedTransform     = componentManager->
					GetComponent<cm::transform>(comparedEntityRefCollider)->tPosition;
				vec3 comparedTransformUpper = componentManager->
					GetComponent<cm::transform>(comparedEntityRefCollider)->tPosition;
				float comparedScale     = componentManager->
					GetComponent<cm::transform>(comparedEntityRefCollider)->fScale;
				for ( unsigned int m = 0; m < linkedEntitiesVectorSizeWithMove; ++m) {
					if ( backtrackingEntityRefCollider == linkedEntitiesWithMove[m] ) {
//						std::cout << "backtrack" << std::endl;
						cm::move* backtrackingMove = componentManager->
							GetComponent<cm::move>(backtrackingEntityRefCollider);
						backtrackingTransform += Normalize(backtrackingMove->frameMovement) * cameraSpeed;
						backtrackingTransform += backtrackingMove->gravity;
					}
				}
				for ( unsigned int n = 0; n < linkedEntitiesVectorSizeWithMove; ++n) {
					if ( comparedEntityRefCollider == linkedEntitiesWithMove[n] ) {
//						std::cout << "compared" << std::endl;
						cm::move* comparedMove     = componentManager->
							GetComponent<cm::move>(comparedEntityRefCollider);
						comparedTransform += Normalize(comparedMove->frameMovement) * cameraSpeed;
						comparedTransform += comparedMove->gravity;
					}
				}
				
				bool boxColliderFlag;
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
//					std::cout << "TEST 1" << std::endl;
                    componentManager->GetComponent<cm::collider>(backtrackingEntityRefCollider)->bGround_Collision_ = true;
//                    componentManager->GetComponent<cm::collider>(comparedEntityRefCollider)->roofCollision = true;
                    continue;
                }
                    
                if(boxColliderFlag) {
//					std::cout << "TEST 2" << std::endl;
                    componentManager->GetComponent<cm::collider>(backtrackingEntityRefCollider)->bWall_Collision_ = true;
//                    componentManager->GetComponent<cm::collider>(comparedEntityRefCollider)->bWall_Collision_ = true;
                    continue;
                }
//				std::cout << "No collissions" << std::endl;
			}
		}
	}

}
