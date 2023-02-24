#include "Systems/MovementSystem.hpp"
#include "ComponentManager.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ControllerComponent.hpp"
#include "Components/DirectionalLightComponent.hpp"
#include "Components/EventComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/PointLightComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/SpotLightComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/ProjectileComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Engine.hpp"
#include "EntityManager.hpp"
#include "Event.hpp"
#include "ISoundEngine.hpp"
#include "Stack.hpp"
#include "Vector.hpp"
#include <cstdio>

namespace GLVM::ecs
{
    CMovementSystem::CMovementSystem(core::CStack& inputStack) :
        inputStack(inputStack) {}
        
    void CMovementSystem::Update()
    {
		namespace cm = GLVM::ecs::components;
		namespace ct = GAME_MECHANICS::ECS::components;
		
        ComponentManager* pComponent_Manager = GLVM::ecs::ComponentManager::GetInstance();
		core::vector<Entity> linkedEntities = pComponent_Manager->collectLinkedEntities<ct::controller,
																						cm::beholder,
																						cm::transform>();
		unsigned int linkedEntitiesVectorSize = linkedEntities.GetSize();
        float cameraSpeed = 5.5f * deltaFrameTime;            

        for(unsigned int i = 0; i < linkedEntitiesVectorSize; ++i) {
            for(int n = 0; n < 6; ++n) {
                bool diagonalMovementFlag = false;

				Entity currentEntity                = linkedEntities[i];
				cm::beholder* beholderComponent     = pComponent_Manager->GetComponent<cm::beholder>(currentEntity);
				cm::transform* transformComponent   = pComponent_Manager->GetComponent<cm::transform>(currentEntity);
		
				vec3 right;
				if(TestDiagonalMovement(inputStack, core::EEvents::eMOVE_BACKWARD, core::EEvents::eMOVE_RIGHT)) {
					right = CalculatePerdendicularVectors(cameraSpeed, *beholderComponent, g_eEvent);
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement =
						Normalize(beholderComponent->forward - right) * cameraSpeed;
					transformComponent->tPosition -=
						Normalize(beholderComponent->forward - right) * cameraSpeed;
					diagonalMovementFlag = true;
				}
				if(TestDiagonalMovement(inputStack, core::EEvents::eMOVE_FORWARD, core::EEvents::eMOVE_RIGHT)) {
					right = CalculatePerdendicularVectors(cameraSpeed, *beholderComponent, g_eEvent);
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement =
						-Normalize(beholderComponent->forward + right) * cameraSpeed;
					transformComponent->tPosition +=
						Normalize(beholderComponent->forward + right) * cameraSpeed;
					diagonalMovementFlag = true;
				}
				if(TestDiagonalMovement(inputStack, core::EEvents::eMOVE_FORWARD, core::EEvents::eMOVE_LEFT)) {
					right = CalculatePerdendicularVectors(cameraSpeed, *beholderComponent, g_eEvent);
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement =
						-Normalize(beholderComponent->forward - right) * cameraSpeed;
					transformComponent->tPosition +=
						Normalize(beholderComponent->forward - right) * cameraSpeed;
					diagonalMovementFlag = true;
				}
				if(TestDiagonalMovement(inputStack, core::EEvents::eMOVE_BACKWARD, core::EEvents::eMOVE_LEFT)) {
					right = CalculatePerdendicularVectors(cameraSpeed, *beholderComponent, g_eEvent);
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement =
						Normalize(beholderComponent->forward + right) * cameraSpeed;
					transformComponent->tPosition -=
						Normalize(beholderComponent->forward + right) * cameraSpeed;
					diagonalMovementFlag = true;
				}
				
                if(diagonalMovementFlag)
                    break;

				vec3 forward;
                switch(inputStack[n])
                {
                case core::EEvents::eMOVE_LEFT:
					right = CalculateVectorRL(*beholderComponent);
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement =
						right * cameraSpeed;
					transformComponent->tPosition -= right * cameraSpeed;
                    break;
                case core::EEvents::eMOVE_RIGHT:
					right = CalculateVectorRL(*beholderComponent);
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement =
						-right * cameraSpeed;
					transformComponent->tPosition += right * cameraSpeed;
                    break;
                case core::EEvents::eMOVE_BACKWARD:
                    forward = CalculateVectorFB(*beholderComponent, g_eEvent);
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement =
						forward * cameraSpeed;
					transformComponent->tPosition -= forward * cameraSpeed;
                    break;
                case core::EEvents::eMOVE_FORWARD:
					forward = CalculateVectorFB(*beholderComponent, g_eEvent);
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement =
						-forward * cameraSpeed;
					transformComponent->tPosition += forward * cameraSpeed;
                    break;
                case core::EEvents::eJUMP:
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement[1] =
						-1.0f;
					transformComponent->tPosition[1] += 1.0f;
                    break;
                default:
                    break;
                }
				
			  	// FIXME: FLASH LIGHT CRUTCH
				
				// core::vector<unsigned int>* pEntityContainerRefSpotLight = ecs::GetEntityContainer<ecs::spotLight>(*pComponent_Manager);
				// unsigned int spotLightComponentContainerSize = pEntityContainerRefSpotLight->GetSize();
				// for(int x = 0; x < spotLightComponentContainerSize; ++x) {
				// 	unsigned int uiSpotLightEntity = (*pEntityContainerRefSpotLight)[x];
				// 	ecs::spotLight& spotLightComponent = pComponent_Manager->GetComponent<ecs::spotLight>(uiSpotLightEntity);
				// 	spotLightComponent.direction = rTransformComponent.tForward;
				// 	spotLightComponent.position  = rTransformComponent.tPosition;
				// }
            }
        }
		
//         for(int n = 0; n < pComponent_Manager->GetEntityContainer<cm::rigidBody>()->GetSize(); ++n) {
// //            int iEntity_refRigidBody = (*ecs::GetEntityContainer<ecs::rigidBody>(*pComponent_Manager))[n];
// 			int iEntity_refRigidBody = (*pComponent_Manager->GetEntityContainer<cm::rigidBody>())[n];
// //            ecs::transform& rTransform_Component = pComponent_Manager->GetComponent<ecs::transform>(iEntity_refRigidBody);
// //			cm::transform& rTransform_Component = pComponent_Manager->GetComponent<cm::transform>(iEntity_refRigidBody);
// 			pComponent_Manager->CreateComponent<cm::move>(iEntity_refRigidBody);
// 			cm::move& move = pComponent_Manager->GetComponent<cm::move>(iEntity_refRigidBody);
//             Vector<float, 3> vec(0.0f);
//             vec[1] = -1.0f;
// //            rTransform_Component.tPosition += vec * cameraSpeed;
// 			move.frameMovement += vec * cameraSpeed;
// //			rTransform_Component.frameVerticalMovement -= vec * cameraSpeed;
//         }
    }

    bool CMovementSystem::TestDiagonalMovement(core::CStack& inputStack,
                                           core::EEvents event1,
                                           core::EEvents event2) {
		core::EEvents availableEvent1 = inputStack.SearchElement(event1);
        core::EEvents availableEvent2 = inputStack.SearchElement(event2);
            
        if((availableEvent1 == event1) && (availableEvent2 == event2))
            return true;
        else
            return false;
    }
        
    Vector<float, 3> CMovementSystem::CalculateVectorRL(components::beholder& beholder) {
        Vector<float, 3> normalizedVector = Normalize(Cross(beholder.forward, beholder.up));
        return normalizedVector;
    }

    Vector<float, 3> CMovementSystem::CalculateVectorFB(components::beholder& beholder,
                                                        core::CEvent& event) {
        Vector<float, 3> forward(0.0f);
        forward[0] = std::cos(Radians(event.mousePointerPosition.yaw));
        forward[2] = std::sin(Radians(event.mousePointerPosition.yaw));
        beholder.forward = Normalize(forward);
        return beholder.forward;
    }

    vec3 CMovementSystem::CalculatePerdendicularVectors(float speed,
                                                        components::beholder& beholder,
                                                        core::CEvent& event) {
		vec3 right(0.0f);
        vec3 forward(0.0f);
        beholder.forward[1] = 0.0f;                                ///< Movement must be only in horizontal vector.
        forward[0] = std::cos(Radians(event.mousePointerPosition.yaw));
        forward[2] = std::sin(Radians(event.mousePointerPosition.yaw));
        beholder.forward = Normalize(forward) * speed;
        right = Normalize(Cross(beholder.forward, beholder.up)) * speed;
		return right;
    }
}


