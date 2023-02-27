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
#include "VertexMath.hpp"
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
			// std::cout << "i: " << i << std::endl;
			// std::cout << "size: " << linkedEntitiesVectorSize << std::endl;
			Entity currentEntity                = linkedEntities[i];
			cm::beholder* beholderComponent     = pComponent_Manager->GetComponent<cm::beholder>(currentEntity);
			cm::transform* transformComponent   = pComponent_Manager->GetComponent<cm::transform>(currentEntity);
			vec3 result = { 0.0f, 0.0f, 0.0f };
			
            for(int n = 0; n < 6; ++n) {
				vec3 right;
				vec3 forward;
                switch(inputStack[n])
                {
                case core::EEvents::eMOVE_LEFT:
					right = CalculateVectorRL(*beholderComponent);
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement +=
						right * cameraSpeed;
					result -= right * cameraSpeed;
                    break;
                case core::EEvents::eMOVE_RIGHT:
					right = CalculateVectorRL(*beholderComponent);
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement +=
						-right * cameraSpeed;
					result += right * cameraSpeed;
                    break;
                case core::EEvents::eMOVE_BACKWARD:
                    forward = CalculateVectorFB(*beholderComponent, g_eEvent);
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement +=
						forward * cameraSpeed;
					result -= forward * cameraSpeed;
                    break;
                case core::EEvents::eMOVE_FORWARD:
					forward = CalculateVectorFB(*beholderComponent, g_eEvent);
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement +=
						-forward * cameraSpeed;
					result += forward * cameraSpeed;
                    break;
                case core::EEvents::eJUMP:
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					transformComponent->tPosition[1] += 5.0f * cameraSpeed;
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
			// std::cout << "x: " << transformComponent->tPosition[0] << std::endl;
			// std::cout << "y: " << transformComponent->tPosition[1] << std::endl;
			// std::cout << "z: " << transformComponent->tPosition[2] << std::endl;

			std::cout << "position: " << Normalize(result) * cameraSpeed << std::endl;
			transformComponent->tPosition += Normalize(result) * cameraSpeed;

			// std::cout << "x: " << transformComponent->tPosition[0] << std::endl;
			// std::cout << "y: " << transformComponent->tPosition[1] << std::endl;
			// std::cout << "z: " << transformComponent->tPosition[2] << std::endl;

        }
		// FIXME: NO NEED TO HAVE SPECIAL FIELD FOR GRAVITY FRAME MOVEMENT
        for(unsigned int n = 0; n < pComponent_Manager->GetEntityContainer<cm::rigidBody>()->GetSize(); ++n) {
//            int iEntity_refRigidBody = (*ecs::GetEntityContainer<ecs::rigidBody>(*pComponent_Manager))[n];
			int iEntity_refRigidBody = (*pComponent_Manager->GetEntityContainer<cm::rigidBody>())[n];
//            ecs::transform& rTransform_Component = pComponent_Manager->GetComponent<ecs::transform>(iEntity_refRigidBody);
			cm::transform* rTransform_Component = pComponent_Manager->GetComponent<cm::transform>(iEntity_refRigidBody);
			pComponent_Manager->CreateComponent<cm::move>(iEntity_refRigidBody);
            // Vector<float, 3> vec(0.0f);
            // vec[1] = -1.0f;
//            rTransform_Component.tPosition += vec * cameraSpeed;
			pComponent_Manager->GetComponent<cm::move>(iEntity_refRigidBody)->gravity[1] = 1.0f * cameraSpeed;
			rTransform_Component->tPosition[1] -= 1.0f * cameraSpeed;
//			rTransform_Component.frameVerticalMovement -= vec * cameraSpeed;
        }
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
}


