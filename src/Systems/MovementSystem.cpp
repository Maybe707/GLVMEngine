#include "Systems/MovementSystem.hpp"
#include "ComponentManager.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/ProjectileComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Engine.hpp"
#include "EntityManager.hpp"
#include "Event.hpp"
#include "ISoundEngine.hpp"
#include "Stack.hpp"

namespace GLVM::ECS
{
    CMovementSystem::CMovementSystem(Core::CStack& _input_Stack, Core::Sound::ISoundEngine* _sound_Engine) :
        Input_Stack_(_input_Stack), Sound_Engine_(_sound_Engine) {}
        
    void CMovementSystem::Update()
    {
        CComponentManager* pComponent_Manager = GLVM::ECS::CComponentManager::GetInstance();
        Core::TCVectorContainer<unsigned int>* pEntity_Container_refMove =
            ECS::GetInnerIDsContainer<ECS::SMoveComponent>(*pComponent_Manager);
        unsigned int u_iVector_Move_Size = pEntity_Container_refMove->GetSize();

        Core::TCVectorContainer<unsigned int>* pEntity_Container_refView =
            ECS::GetInnerIDsContainer<ECS::CViewComponent>(*pComponent_Manager);
        unsigned int iEntity_refView = (*pEntity_Container_refView)[0];
        ECS::CViewComponent& view_Component = pComponent_Manager->GetComponent<ECS::CViewComponent>(iEntity_refView);

        float cameraSpeed = 5.5f * _dOffset;            
        // int counter = 0;

        // if(fProjectile_Accumulator_ > 0)
        //     fProjectile_Accumulator_ -= cameraSpeed;
        
        for(unsigned int i = 0; i < u_iVector_Move_Size; ++i) {
            for(int n = 0; n < 6; ++n) {
                bool bDiagonal_Movement_Availability = false;
                
                unsigned int iEntity_refMove = (*pEntity_Container_refMove)[i];
                ECS::STransformComponent& rTransformComponent = pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refMove);
                
                // if(Input_Stack_.SearchElement(Core::EEvents::eMOUSE_LEFT_BUTTON) == Core::EEvents::eMOUSE_LEFT_BUTTON) {
                //     if(fProjectile_Accumulator_ <= 0) {
                //         CalculateProjectile(pComponent_Manager,
                //                             iEntity_refMove,
                //                             view_Component);
                //         fProjectile_Accumulator_ = 2.0;
                //     }
                // }
                bDiagonal_Movement_Availability = FixDiagonalMove(Input_Stack_,
                                                                  rTransformComponent,
                                                                  cameraSpeed,
                                                                  view_Component,
                                                                  g_eEvent);
//                std::cout << Input_Stack_[n] << std::endl;
                if(bDiagonal_Movement_Availability)
                    break;
                switch(Input_Stack_[n])
                {
                case Core::EEvents::eMOVE_LEFT:
                    rTransformComponent.tRight =
                        CalculateVectorRL(view_Component);
                    rTransformComponent.tPosition -= rTransformComponent.tRight * cameraSpeed;
                    break;
                case Core::EEvents::eMOVE_RIGHT:
                    rTransformComponent.tRight =
                        CalculateVectorRL(view_Component);
                    rTransformComponent.tPosition += rTransformComponent.tRight * cameraSpeed;
                    break;
                case Core::EEvents::eMOVE_BACKWARD:
                    rTransformComponent.tForward =
                        CalculateVectorFB(view_Component,
                                          g_eEvent);
                    rTransformComponent.tPosition -= rTransformComponent.tForward * cameraSpeed;
                    break;
                case Core::EEvents::eMOVE_FORWARD:
                    rTransformComponent.tForward =
                        CalculateVectorFB(view_Component,
                                          g_eEvent);
                    rTransformComponent.tPosition += rTransformComponent.tForward * cameraSpeed;
                    break;
                case Core::EEvents::eJUMP:
                    pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refMove).tPosition[1] += 1.0f;
                    pComponent_Manager->GetComponent<ECS::SMoveComponent>(iEntity_refMove).eEvent_ = Core::EEvents::eJUMP;
                    break;
                default:
                    break;
                }
				// Core::TCVectorContainer<unsigned int>* pEntityContainerRefSpotLight = ECS::GetInnerIDsContainer<ECS::SSpotLightComponent>(*pComponent_Manager);
				// unsigned int spotLightComponentContainerSize = pEntityContainerRefSpotLight->GetSize();
				// for(int x = 0; x < spotLightComponentContainerSize; ++x) {
				// 	unsigned int uiSpotLightEntity = (*pEntityContainerRefSpotLight)[x];
				// 	ECS::SSpotLightComponent& spotLightComponent = pComponent_Manager->GetComponent<ECS::SSpotLightComponent>(uiSpotLightEntity);
				// 	spotLightComponent.direction = rTransformComponent.tForward;
				// 	spotLightComponent.position  = rTransformComponent.tPosition;
				// }
            }
        }

		
        // for(int n = 0; n < ECS::GetInnerIDsContainer<ECS::CRigidBodyComponent>(*pComponent_Manager)->GetSize(); ++n) {
        //     int iEntity_refRigidBody = (*ECS::GetInnerIDsContainer<ECS::CRigidBodyComponent>(*pComponent_Manager))[n];
        //     ECS::STransformComponent& rTransform_Component = pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refRigidBody);
        //     Vector<float, 3> vec(0.0f);
        //     vec[1] = -1.0f;
        //     rTransform_Component.tPosition += vec * cameraSpeed;
        // }
    }

    bool CMovementSystem::CompareDirection(Core::CStack& _input_Stack,
                                           Core::EEvents _event0,
                                           Core::EEvents _event1) {
        Core::EEvents eTemp_Event0;
        Core::EEvents eTemp_Event1;

        eTemp_Event0 = _input_Stack.SearchElement(_event0);
        eTemp_Event1 = _input_Stack.SearchElement(_event1);
            
        if((eTemp_Event0 == _event0) && (eTemp_Event1 == _event1))
            return true;
        else
            return false;
    }
        
    void CMovementSystem::DiscardOpposite(Core::CStack& _input_Stack)
    {
        if(CompareDirection(_input_Stack, Core::EEvents::eMOVE_LEFT, Core::EEvents::eMOVE_RIGHT))
        {
            _input_Stack.Remove(Core::EEvents::eMOVE_LEFT);
            _input_Stack.Remove(Core::EEvents::eMOVE_RIGHT);
        }
        if(CompareDirection(_input_Stack, Core::EEvents::eMOVE_BACKWARD, Core::EEvents::eMOVE_FORWARD))
        {
            _input_Stack.Remove(Core::EEvents::eMOVE_BACKWARD);
            _input_Stack.Remove(Core::EEvents::eMOVE_FORWARD);
        }
    }

    Vector<float, 3> CMovementSystem::CalculateVectorRL(ECS::CViewComponent& _view_Component) {
        Vector<float, 3> tNormalized_Vector = Normalize(Cross(_view_Component.Front_Camera, _view_Component.Up_Camera));
        return tNormalized_Vector;
    }

    Vector<float, 3> CMovementSystem::CalculateVectorFB(ECS::CViewComponent& _view_Component,
                                                        Core::CEvent& _event) {
        Vector<float, 3> front(0.0f);
        front[0] = std::cos(Radians(_event.mouse_Pointer_Position_.fYaw_));
        front[2] = std::sin(Radians(_event.mouse_Pointer_Position_.fYaw_));
        _view_Component.Front_Camera = Normalize(front);
        return _view_Component.Front_Camera;
    }

    void CMovementSystem::CalculatePerdendicularVectors(float _camera_Speed,
                                                        ECS::CViewComponent& _view_Component,
                                                        Core::CEvent& _event,
                                                        Vector<float, 3>& _temp_Vector) {
        Vector<float, 3> front(0.0f);
        _view_Component.Front_Camera[1] = 0.0f;
        front[0] = std::cos(Radians(_event.mouse_Pointer_Position_.fYaw_));
        front[2] = std::sin(Radians(_event.mouse_Pointer_Position_.fYaw_));
        _view_Component.Front_Camera = Normalize(front) * _camera_Speed;
        _temp_Vector = Normalize(Cross(_view_Component.Front_Camera, _view_Component.Up_Camera)) * _camera_Speed;
    }
        
    bool CMovementSystem::FixDiagonalMove(Core::CStack& _input_Stack,
                                          STransformComponent& _transform_Component,
                                          float _camera_Speed,
                                          ECS::CViewComponent& _view_Component,
                                          Core::CEvent& _event) {
        Vector<float, 3> temp_Vector(0.0f);
        if(CompareDirection(_input_Stack, Core::EEvents::eMOVE_BACKWARD, Core::EEvents::eMOVE_RIGHT))
        {
            CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
            _transform_Component.tPosition -= Normalize(_view_Component.Front_Camera - temp_Vector) * _camera_Speed;
            return true;
        }
        if(CompareDirection(_input_Stack, Core::EEvents::eMOVE_FORWARD, Core::EEvents::eMOVE_RIGHT))
        {
            CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
            _transform_Component.tPosition += Normalize(temp_Vector + _view_Component.Front_Camera) * _camera_Speed;
            return true;
        }
        if(CompareDirection(_input_Stack, Core::EEvents::eMOVE_FORWARD, Core::EEvents::eMOVE_LEFT))
        {
            CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
            _transform_Component.tPosition += Normalize(_view_Component.Front_Camera - temp_Vector) * _camera_Speed;
            return true;
        }
        if(CompareDirection(_input_Stack, Core::EEvents::eMOVE_BACKWARD, Core::EEvents::eMOVE_LEFT))
        {
            CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
            _transform_Component.tPosition -= Normalize(_view_Component.Front_Camera + temp_Vector) * _camera_Speed;
            return true;
        }
        return false;
    }
}


