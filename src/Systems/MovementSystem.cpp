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
    CMovementSystem::CMovementSystem(core::CStack& _input_Stack, core::Sound::ISoundEngine* _sound_Engine) :
        Input_Stack_(_input_Stack), Sound_Engine_(_sound_Engine) {}
        
    void CMovementSystem::Update()
    {
		namespace cm = GLVM::ecs::components;
		namespace ct = GAME_MECHANICS::ECS::components;

        CComponentManager* pComponent_Manager = GLVM::ecs::CComponentManager::GetInstance();
        // core::vector<unsigned int>* controllerComponents =
		// 	pComponent_Manager->GetEntityContainer<ct::controller>();
        // unsigned int entitiesVectorSize = controllerComponents->GetSize();

		core::vector<Entity> entitiesVector = pComponent_Manager->collectEntities<cm::collider, cm::beholder>();
		unsigned int entitiesVectorSize = entitiesVector.GetSize();
		// for ( unsigned int i = 0; i < vectorWithEntities.GetSize(); ++i)
		// 	std::cout << vectorWithEntities[i] << std::endl;
		
        // core::vector<unsigned int>* pEntity_Container_refView =
		// 	pComponent_Manager->GetEntityContainer<cm::beholder>();

        float cameraSpeed = 5.5f * _dOffset;            

        for(unsigned int i = 0; i < entitiesVectorSize; ++i) {
            for(int n = 0; n < 6; ++n) {
                bool diagonalMovementAvailability = false;

//                unsigned int currentEntity = (*controllerComponents)[i];
				// unsigned int entityRefView = (*pEntity_Container_refView)[i];
				Entity currentEntity                = entitiesVector[i];
				cm::beholder* beholderComponent     = pComponent_Manager->GetComponent<cm::beholder>(currentEntity);
//				cm::beholder* colliderComponent = pComponent_Manager->GetComponent<cm::beholder>(currentEntity);
		
				vec3 right;
				if(CompareDirection(Input_Stack_, core::EEvents::eMOVE_BACKWARD, core::EEvents::eMOVE_RIGHT)) {
					right = CalculatePerdendicularVectors(cameraSpeed, *beholderComponent, g_eEvent);
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement =
						-Normalize(beholderComponent->Front_Camera - right) * cameraSpeed;
					diagonalMovementAvailability = true;
				}
				if(CompareDirection(Input_Stack_, core::EEvents::eMOVE_FORWARD, core::EEvents::eMOVE_RIGHT)) {
					right = CalculatePerdendicularVectors(cameraSpeed, *beholderComponent, g_eEvent);
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement =
						Normalize(beholderComponent->Front_Camera + right) * cameraSpeed;
					diagonalMovementAvailability = true;
				}
				if(CompareDirection(Input_Stack_, core::EEvents::eMOVE_FORWARD, core::EEvents::eMOVE_LEFT)) {
					right = CalculatePerdendicularVectors(cameraSpeed, *beholderComponent, g_eEvent);
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement =
						Normalize(beholderComponent->Front_Camera - right) * cameraSpeed;
					diagonalMovementAvailability = true;
				}
				if(CompareDirection(Input_Stack_, core::EEvents::eMOVE_BACKWARD, core::EEvents::eMOVE_LEFT)) {
					right = CalculatePerdendicularVectors(cameraSpeed, *beholderComponent, g_eEvent);
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement =
						-Normalize(beholderComponent->Front_Camera + right) * cameraSpeed;
					diagonalMovementAvailability = true;
				}
				
                if(diagonalMovementAvailability)
                    break;

				vec3 forward;
                switch(Input_Stack_[n])
                {
                case core::EEvents::eMOVE_LEFT:
					right = CalculateVectorRL(*beholderComponent);
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement +=
						-right * cameraSpeed;					
                    break;
                case core::EEvents::eMOVE_RIGHT:
					right = CalculateVectorRL(*beholderComponent);
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement +=
						right * cameraSpeed;
                    break;
                case core::EEvents::eMOVE_BACKWARD:
                    forward = CalculateVectorFB(*beholderComponent, g_eEvent);
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement +=
						-forward * cameraSpeed;
                    break;
                case core::EEvents::eMOVE_FORWARD:
					forward = CalculateVectorFB(*beholderComponent, g_eEvent);
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement +=
						forward * cameraSpeed;
                    break;
                case core::EEvents::eJUMP:
					pComponent_Manager->CreateComponent<cm::move>(currentEntity);
					pComponent_Manager->GetComponent<cm::move>(currentEntity)->frameMovement[1] +=
						1.0f;
                    break;
                default:
                    break;
                }
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

    bool CMovementSystem::CompareDirection(core::CStack& _input_Stack,
                                           core::EEvents _event0,
                                           core::EEvents _event1) {
		core::EEvents eTemp_Event0 = _input_Stack.SearchElement(_event0);
        core::EEvents eTemp_Event1 = _input_Stack.SearchElement(_event1);
            
        if((eTemp_Event0 == _event0) && (eTemp_Event1 == _event1))
            return true;
        else
            return false;
    }
        
    Vector<float, 3> CMovementSystem::CalculateVectorRL(components::beholder& _view_Component) {
        Vector<float, 3> tNormalized_Vector = Normalize(Cross(_view_Component.Front_Camera, _view_Component.Up_Camera));
        return tNormalized_Vector;
    }

    Vector<float, 3> CMovementSystem::CalculateVectorFB(components::beholder& _view_Component,
                                                        core::CEvent& _event) {
        Vector<float, 3> front(0.0f);
        front[0] = std::cos(Radians(_event.mouse_Pointer_Position_.fYaw_));
        front[2] = std::sin(Radians(_event.mouse_Pointer_Position_.fYaw_));
        _view_Component.Front_Camera = Normalize(front);
        return _view_Component.Front_Camera;
    }

    vec3 CMovementSystem::CalculatePerdendicularVectors(float speed,
                                                        components::beholder& beholder,
                                                        core::CEvent& _event) {
		vec3 right(0.0f);
        vec3 front(0.0f);
        beholder.Front_Camera[1] = 0.0f;
        front[0] = std::cos(Radians(_event.mouse_Pointer_Position_.fYaw_));
        front[2] = std::sin(Radians(_event.mouse_Pointer_Position_.fYaw_));
        beholder.Front_Camera = Normalize(front) * speed;
        right = Normalize(Cross(beholder.Front_Camera, beholder.Up_Camera)) * speed;
		return right;
    }
        
    // bool CMovementSystem::FixDiagonalMove(core::CStack& _input_Stack,
    //                                       components::transform& _transform_Component,
    //                                       float _camera_Speed,
    //                                       components::beholder& _view_Component,
	// 									  components::move& moveComponent,
    //                                       core::CEvent& _event) {
	// 	namespace cm = GLVM::ecs::components;
    //     CComponentManager* pComponent_Manager = GLVM::ecs::CComponentManager::GetInstance();		
    //     Vector<float, 3> temp_Vector(0.0f);
    //     if(CompareDirection(_input_Stack, core::EEvents::eMOVE_BACKWARD, core::EEvents::eMOVE_RIGHT))
    //     {
    //         CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
    //         _transform_Component.tPosition -= Normalize(_view_Component.Front_Camera - temp_Vector) * _camera_Speed;
	// 		pComponent_Manager->CreateComponent<cm::move>(iEntity_refMove);
	// 		pComponent_Manager->GetComponent<cm::move>(iEntity_refMove).frameHorizontalMovement
	// 			_transform_Component.frameHorizontalMovement += Normalize(_view_Component.Front_Camera - temp_Vector) * _camera_Speed;
    //         return true;
    //     }
    //     if(CompareDirection(_input_Stack, core::EEvents::eMOVE_FORWARD, core::EEvents::eMOVE_RIGHT))
    //     {
    //         CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
    //         _transform_Component.tPosition += Normalize(temp_Vector + _view_Component.Front_Camera) * _camera_Speed;
	// 		_transform_Component.frameHorizontalMovement -= Normalize(temp_Vector + _view_Component.Front_Camera) * _camera_Speed;
    //         return true;
    //     }
    //     if(CompareDirection(_input_Stack, core::EEvents::eMOVE_FORWARD, core::EEvents::eMOVE_LEFT))
    //     {
    //         CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
    //         _transform_Component.tPosition += Normalize(_view_Component.Front_Camera - temp_Vector) * _camera_Speed;
	// 		_transform_Component.frameHorizontalMovement -= Normalize(_view_Component.Front_Camera - temp_Vector) * _camera_Speed;
    //         return true;
    //     }
    //     if(CompareDirection(_input_Stack, core::EEvents::eMOVE_BACKWARD, core::EEvents::eMOVE_LEFT))
    //     {
    //         CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
    //         _transform_Component.tPosition -= Normalize(_view_Component.Front_Camera + temp_Vector) * _camera_Speed;
	// 		_transform_Component.frameHorizontalMovement += Normalize(_view_Component.Front_Camera + temp_Vector) * _camera_Speed;
    //         return true;
    //     }
    //     return false;
    // }
}


