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

namespace GLVM::ecs
{
    CMovementSystem::CMovementSystem(core::CStack& _input_Stack, core::Sound::ISoundEngine* _sound_Engine) :
        Input_Stack_(_input_Stack), Sound_Engine_(_sound_Engine) {}
        
    void CMovementSystem::Update()
    {
		namespace cm = GLVM::ecs::components;
		
        CComponentManager* pComponent_Manager = GLVM::ecs::CComponentManager::GetInstance();
        core::TCVectorContainer<unsigned int>* pEntity_Container_refMove =
            ecs::GetInnerIDsContainer<cm::move>(*pComponent_Manager);
        unsigned int u_iVector_Move_Size = pEntity_Container_refMove->GetSize();

        core::TCVectorContainer<unsigned int>* pEntity_Container_refView =
            ecs::GetInnerIDsContainer<cm::beholder>(*pComponent_Manager);
        unsigned int iEntity_refView = (*pEntity_Container_refView)[0];
		cm::beholder& view_Component = pComponent_Manager->GetComponent<cm::beholder>(iEntity_refView);

        float cameraSpeed = 5.5f * _dOffset;            
        // int counter = 0;

        // if(fProjectile_Accumulator_ > 0)
        //     fProjectile_Accumulator_ -= cameraSpeed;
        
        for(unsigned int i = 0; i < u_iVector_Move_Size; ++i) {
            for(int n = 0; n < 6; ++n) {
                bool bDiagonal_Movement_Availability = false;
                
                unsigned int iEntity_refMove = (*pEntity_Container_refMove)[i];
				cm::transform& rTransformComponent = pComponent_Manager->GetComponent<cm::transform>(iEntity_refMove);
                
                // if(Input_Stack_.SearchElement(core::EEvents::eMOUSE_LEFT_BUTTON) == core::EEvents::eMOUSE_LEFT_BUTTON) {
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
                case core::EEvents::eMOVE_LEFT:
                    rTransformComponent.tRight =
                        CalculateVectorRL(view_Component);
                    rTransformComponent.tPosition -= rTransformComponent.tRight * cameraSpeed;
                    break;
                case core::EEvents::eMOVE_RIGHT:
                    rTransformComponent.tRight =
                        CalculateVectorRL(view_Component);
                    rTransformComponent.tPosition += rTransformComponent.tRight * cameraSpeed;
                    break;
                case core::EEvents::eMOVE_BACKWARD:
                    rTransformComponent.tForward =
                        CalculateVectorFB(view_Component,
                                          g_eEvent);
                    rTransformComponent.tPosition -= rTransformComponent.tForward * cameraSpeed;
                    break;
                case core::EEvents::eMOVE_FORWARD:
                    rTransformComponent.tForward =
                        CalculateVectorFB(view_Component,
                                          g_eEvent);
                    rTransformComponent.tPosition += rTransformComponent.tForward * cameraSpeed;
                    break;
                case core::EEvents::eJUMP:
                    pComponent_Manager->GetComponent<cm::transform>(iEntity_refMove).tPosition[1] += 1.0f;
                    pComponent_Manager->GetComponent<cm::move>(iEntity_refMove).eEvent_ = core::EEvents::eJUMP;
                    break;
                default:
                    break;
                }
				// core::TCVectorContainer<unsigned int>* pEntityContainerRefSpotLight = ecs::GetInnerIDsContainer<ecs::spotLight>(*pComponent_Manager);
				// unsigned int spotLightComponentContainerSize = pEntityContainerRefSpotLight->GetSize();
				// for(int x = 0; x < spotLightComponentContainerSize; ++x) {
				// 	unsigned int uiSpotLightEntity = (*pEntityContainerRefSpotLight)[x];
				// 	ecs::spotLight& spotLightComponent = pComponent_Manager->GetComponent<ecs::spotLight>(uiSpotLightEntity);
				// 	spotLightComponent.direction = rTransformComponent.tForward;
				// 	spotLightComponent.position  = rTransformComponent.tPosition;
				// }
            }
        }

		
        // for(int n = 0; n < ecs::GetInnerIDsContainer<ecs::rigidBody>(*pComponent_Manager)->GetSize(); ++n) {
        //     int iEntity_refRigidBody = (*ecs::GetInnerIDsContainer<ecs::rigidBody>(*pComponent_Manager))[n];
        //     ecs::transform& rTransform_Component = pComponent_Manager->GetComponent<ecs::transform>(iEntity_refRigidBody);
        //     Vector<float, 3> vec(0.0f);
        //     vec[1] = -1.0f;
        //     rTransform_Component.tPosition += vec * cameraSpeed;
        // }
    }

    bool CMovementSystem::CompareDirection(core::CStack& _input_Stack,
                                           core::EEvents _event0,
                                           core::EEvents _event1) {
        core::EEvents eTemp_Event0;
        core::EEvents eTemp_Event1;

        eTemp_Event0 = _input_Stack.SearchElement(_event0);
        eTemp_Event1 = _input_Stack.SearchElement(_event1);
            
        if((eTemp_Event0 == _event0) && (eTemp_Event1 == _event1))
            return true;
        else
            return false;
    }
        
    void CMovementSystem::DiscardOpposite(core::CStack& _input_Stack)
    {
        if(CompareDirection(_input_Stack, core::EEvents::eMOVE_LEFT, core::EEvents::eMOVE_RIGHT))
        {
            _input_Stack.Remove(core::EEvents::eMOVE_LEFT);
            _input_Stack.Remove(core::EEvents::eMOVE_RIGHT);
        }
        if(CompareDirection(_input_Stack, core::EEvents::eMOVE_BACKWARD, core::EEvents::eMOVE_FORWARD))
        {
            _input_Stack.Remove(core::EEvents::eMOVE_BACKWARD);
            _input_Stack.Remove(core::EEvents::eMOVE_FORWARD);
        }
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

    void CMovementSystem::CalculatePerdendicularVectors(float _camera_Speed,
                                                        components::beholder& _view_Component,
                                                        core::CEvent& _event,
                                                        Vector<float, 3>& _temp_Vector) {
        Vector<float, 3> front(0.0f);
        _view_Component.Front_Camera[1] = 0.0f;
        front[0] = std::cos(Radians(_event.mouse_Pointer_Position_.fYaw_));
        front[2] = std::sin(Radians(_event.mouse_Pointer_Position_.fYaw_));
        _view_Component.Front_Camera = Normalize(front) * _camera_Speed;
        _temp_Vector = Normalize(Cross(_view_Component.Front_Camera, _view_Component.Up_Camera)) * _camera_Speed;
    }
        
    bool CMovementSystem::FixDiagonalMove(core::CStack& _input_Stack,
                                          components::transform& _transform_Component,
                                          float _camera_Speed,
                                          components::beholder& _view_Component,
                                          core::CEvent& _event) {
        Vector<float, 3> temp_Vector(0.0f);
        if(CompareDirection(_input_Stack, core::EEvents::eMOVE_BACKWARD, core::EEvents::eMOVE_RIGHT))
        {
            CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
            _transform_Component.tPosition -= Normalize(_view_Component.Front_Camera - temp_Vector) * _camera_Speed;
            return true;
        }
        if(CompareDirection(_input_Stack, core::EEvents::eMOVE_FORWARD, core::EEvents::eMOVE_RIGHT))
        {
            CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
            _transform_Component.tPosition += Normalize(temp_Vector + _view_Component.Front_Camera) * _camera_Speed;
            return true;
        }
        if(CompareDirection(_input_Stack, core::EEvents::eMOVE_FORWARD, core::EEvents::eMOVE_LEFT))
        {
            CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
            _transform_Component.tPosition += Normalize(_view_Component.Front_Camera - temp_Vector) * _camera_Speed;
            return true;
        }
        if(CompareDirection(_input_Stack, core::EEvents::eMOVE_BACKWARD, core::EEvents::eMOVE_LEFT))
        {
            CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
            _transform_Component.tPosition -= Normalize(_view_Component.Front_Camera + temp_Vector) * _camera_Speed;
            return true;
        }
        return false;
    }
}


