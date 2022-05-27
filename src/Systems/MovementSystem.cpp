#include "Systems/MovementSystem.hpp"
#include "ComponentManager.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/TextureComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/ProjectileComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Engine.hpp"
#include "EntityManager.hpp"
#include "Event.hpp"
#include "Stack.hpp"

namespace GLVM::ECS
{
    CMovementSystem::CMovementSystem(Core::CStack& _input_Stack) : Input_Stack_(_input_Stack) {}
        
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

        float cameraSpeed = 2.5f * _dOffset;            
        int counter = 0;

        if(fProjectile_Accumulator_ > 0)
            fProjectile_Accumulator_ -= cameraSpeed;
        
        std::cout << "Projectile accumulator: " << fProjectile_Accumulator_ << std::endl;
        
        for(int i = 0; i < u_iVector_Move_Size; ++i)
        {
            for(int n = 0; n < 6; ++n)
            {
                bool bDiagonal_Movement_Availability = false;
                
                unsigned int iEntity_refMove = (*pEntity_Container_refMove)[i];
                
                if(Input_Stack_.SearchElement(Core::EEvents::eMOUSE_LEFT_BUTTON) == Core::EEvents::eMOUSE_LEFT_BUTTON)
                {
                    if(fProjectile_Accumulator_ <= 0)
                    {
                        std::cout << "Signal!!" << std::endl;
                        CalculateProjectile(pComponent_Manager,
                                            iEntity_refMove,
                                            cameraSpeed,
                                            view_Component,
                                            Input_Stack_, n);
                        fProjectile_Accumulator_ = 2.0;
                    }
                }
//                Input_Stack_.PrintStack();
            
                bDiagonal_Movement_Availability = FixDiagonalMove(Input_Stack_,
                                                                  pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refMove),
                                                                  cameraSpeed,
                                                                  view_Component,
                                                                  g_eEvent);
                if(bDiagonal_Movement_Availability)
                    break;
                switch(Input_Stack_[n])
                {
                case Core::EEvents::eMOVE_LEFT:
                    pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refMove).tVertex +=
                        CalculateVectorRL(pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refMove),
                                          view_Component,
                                          pComponent_Manager->GetComponent<ECS::SMoveComponent>(iEntity_refMove),
                                          cameraSpeed,
                                          Input_Stack_[n]);
                    break;
                case Core::EEvents::eMOVE_RIGHT:
                    pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refMove).tVertex +=
                        CalculateVectorRL(pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refMove),
                                          view_Component,
                                          pComponent_Manager->GetComponent<ECS::SMoveComponent>(iEntity_refMove),
                                          cameraSpeed,
                                          Input_Stack_[n]);
                    break;
                case Core::EEvents::eMOVE_BACKWARD:
                    pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refMove).tVertex +=
                        CalculateVectorFB(pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refMove),
                                          pComponent_Manager->GetComponent<ECS::SMoveComponent>(iEntity_refMove),
                                          cameraSpeed,
                                          view_Component,
                                          g_eEvent,
                                          Input_Stack_[n]);
                    break;
                case Core::EEvents::eMOVE_FORWARD:
                    pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refMove).tVertex +=
                        CalculateVectorFB(pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refMove),
                                          pComponent_Manager->GetComponent<ECS::SMoveComponent>(iEntity_refMove),
                                          cameraSpeed,
                                          view_Component,
                                          g_eEvent,
                                          Input_Stack_[n]);
                    break;
                case Core::EEvents::eJUMP:
                    pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refMove).tVertex[1] += 1.0f;
                    pComponent_Manager->GetComponent<ECS::SMoveComponent>(iEntity_refMove).eEvent_ = Core::EEvents::eJUMP;
                    break;
                // case Core::EEvents::eMOUSE_LEFT_BUTTON:

                //     if(fProjectile_Accumulator_ <= 0)
                //     {
                //         std::cout << "Signal!!" << std::endl;
                //         CalculateProjectile(pComponent_Manager,
                //                             iEntity_refMove,
                //                             cameraSpeed,
                //                             view_Component,
                //                             Input_Stack_, n);
                //         fProjectile_Accumulator_ = 10.0;
                //     }
                //     break;
                default:
                    break;
                }
            }
        }
    }

    void CMovementSystem::CalculateProjectile(ECS::CComponentManager* pComponent_Manager,
                                              unsigned int iEntity_refMove,
                                              float cameraSpeed,
                                              CViewComponent& view_Component,
                                              Core::CStack& Input_Stack_,
                                              int n)
    {
        unsigned int uiEntity_Projectile;
        ECS::CEntityManager::GetInstance()->CreateEntity(uiEntity_Projectile);
        ECS::CComponentManager::GetInstance()->CreateComponent<ECS::SVertexComponent, ECS::CColliderComponent,
                                                               ECS::STransformComponent, ECS::CTextureComponent,
                                                               ECS::CProjectileComponent>(uiEntity_Projectile);
        ECS::CTextureComponent& rTextureProjectile = pComponent_Manager->GetComponent<ECS::CTextureComponent>(uiEntity_Projectile);
        rTextureProjectile.iWidth_  = 96;
        rTextureProjectile.iHeight_ = 128;
        rTextureProjectile.u_iData_ = chelik_dat;
        Core::CEngine::GetInstance()->LoadTextureData(rTextureProjectile);
        ECS::STransformComponent& rTransformProjectile = pComponent_Manager->GetComponent<ECS::STransformComponent>(uiEntity_Projectile);
        ECS::CProjectileComponent& rProjectileComponent = pComponent_Manager->GetComponent<ECS::CProjectileComponent>(uiEntity_Projectile);
        rTransformProjectile.fScale = 0.5f;
        rTransformProjectile.tVertex[0] = 0.5f;
        rTransformProjectile.tVertex[1] = 0.5f;
        rTransformProjectile.tVertex[2] = 0.5f;
        // rTransformProjectile.tVertex = CalculateForwardVectorProjectile(
        //     pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refMove),
        //     pComponent_Manager->GetComponent<ECS::SMoveComponent>(iEntity_refMove),
        //     cameraSpeed,
        //     view_Component,
        //     g_eEvent,
        //     Input_Stack_[n]
        //     );
        // Vector<float, 3> temp_vec(0.0f);
        // temp_vec = pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refMove).tVertex;
        // temp_vec *= cameraSpeed;
        // rTransformProjectile.tVertex = temp_vec;
        Vector<float, 3> vec(0.0f);
       rTransformProjectile.tVertex = pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refMove).tVertex;
       rProjectileComponent.forward_= GetDirectionVector(pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refMove),
                                                         view_Component);
       // vec = rTransformProjectile.tVertex + rProjectileComponent.forward_;
       // rTransformProjectile.tVertex = (vec - rTransformProjectile.tVertex);
//       rTransformProjectile.tVertex[2] += 0.5f;
//       rTransformProjectile.tVertex[1] += 0.5f;
//       rTransformProjectile.tVertex[0] += 0.25f;
    }

    Vector<float, 3> CMovementSystem::GetDirectionVector(ECS::STransformComponent& _Player, ECS::CViewComponent& _view_Component)
    {
                Matrix<float, 4> tView_Matrix(1.0f);
        const float kSensitivity = 0.1f;

        fYaw = g_eEvent.mouse_Pointer_Position_.iOffset_X;
        fPitch = g_eEvent.mouse_Pointer_Position_.iOffset_Y;
        fYaw *= kSensitivity;
        fPitch *= kSensitivity;

        g_eEvent.mouse_Pointer_Position_.fPitch_ = fPitch;
        g_eEvent.mouse_Pointer_Position_.fYaw_ = fYaw;
        
        if(fPitch > 89.0f)
            fPitch = 89.0f;
        if(fPitch < -89.0f)
            fPitch = -89.0f;

        Vector<float, 3> front;
        front[0] = std::cos(Radians(fYaw)) * std::cos(Radians(fPitch));
        front[1] = std::sin(Radians(fPitch));
        front[2] = std::sin(Radians(fYaw)) * std::cos(Radians(fPitch));
        _view_Component.Front_Camera = Normalize(front);

        return _view_Component.Front_Camera;
    }
    
    bool CMovementSystem::CompareDirection(Core::CStack& _input_Stack,
                                           Core::EEvents _event0,
                                           Core::EEvents _event1)
    {
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

    Vector<float, 3> CMovementSystem::CalculateVectorRL(STransformComponent _transform_Component,
                                                        ECS::CViewComponent& _view_Component,
                                                        SMoveComponent& _move_Component,
                                                        float _camera_Speed,
                                                        Core::EEvents _current_Event)
    {
        Vector<float, 3> temp_Vector(0.0f);
        Vector<float, 3> normalized_Vector = Normalize(Cross(_view_Component.Front_Camera, _view_Component.Up_Camera));
        if(_current_Event == Core::EEvents::eMOVE_RIGHT)
            temp_Vector +=  normalized_Vector * _camera_Speed;
        else
            temp_Vector -= normalized_Vector * _camera_Speed;
        _move_Component.eEvent_ = _current_Event;
        return temp_Vector;
    }

    Vector<float, 3> CMovementSystem::CalculateVectorFB(STransformComponent _transform_Component,
                                                        SMoveComponent& _move_Component,
                                                        float _camera_Speed,
                                                        ECS::CViewComponent& _view_Component,
                                                        Core::CEvent& _event,
                                                        Core::EEvents _current_Event)
    {
        Vector<float, 3> front(0.0f);
        Vector<float, 3> temp_Vector(0.0f);
//        _view_Component.Front_Camera[1] = 0.0f;
        front[0] = std::cos(Radians(_event.mouse_Pointer_Position_.fYaw_));
        front[2] = std::sin(Radians(_event.mouse_Pointer_Position_.fYaw_));
        _view_Component.Front_Camera = Normalize(front);
        if(_current_Event == Core::EEvents::eMOVE_FORWARD)
            temp_Vector += _view_Component.Front_Camera * _camera_Speed;
        else
            temp_Vector -= _view_Component.Front_Camera * _camera_Speed;
        _move_Component.eEvent_ = _current_Event;
        return temp_Vector;
    }

    Vector<float, 3> CMovementSystem::CalculateForwardVectorProjectile(STransformComponent _transform_Component,
                                                                       SMoveComponent& _move_Component,
                                                                       float _camera_Speed,
                                                                       ECS::CViewComponent& _view_Component,
                                                                       Core::CEvent& _event,
                                                                       Core::EEvents _current_Event)
    {
        Vector<float, 3> front(0.0f);
        Vector<float, 3> temp_Vector(0.0f);
        // _view_Component.Front_Camera[1] = 0.0f;
        // front[0] = std::cos(Radians(_event.mouse_Pointer_Position_.fYaw_));
        // front[2] = std::sin(Radians(_event.mouse_Pointer_Position_.fYaw_));
        front[0] = std::cos(Radians(_event.mouse_Pointer_Position_.fYaw_)) * std::cos(Radians(_event.mouse_Pointer_Position_.fPitch_));
        front[1] = std::sin(Radians(_event.mouse_Pointer_Position_.fPitch_));
        front[2] = std::sin(Radians(_event.mouse_Pointer_Position_.fYaw_)) * std::cos(Radians(_event.mouse_Pointer_Position_.fPitch_));
        _view_Component.Front_Camera = Normalize(front);
        temp_Vector += _view_Component.Front_Camera * _camera_Speed;
        _move_Component.eEvent_ = _current_Event;
        return temp_Vector;
    }
    
    void CMovementSystem::CalculatePerdendicularVectors(float _camera_Speed,
                                                        ECS::CViewComponent& _view_Component,
                                                        Core::CEvent& _event,
                                                        Vector<float, 3>& _temp_Vector)
    {
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
                                          Core::CEvent& _event)
    {
        Vector<float, 3> temp_Vector(0.0f);
        if(CompareDirection(_input_Stack, Core::EEvents::eMOVE_BACKWARD, Core::EEvents::eMOVE_RIGHT))
        {
            CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
            _transform_Component.tVertex -= Normalize(_view_Component.Front_Camera - temp_Vector) * _camera_Speed;
            return true;
        }
        if(CompareDirection(_input_Stack, Core::EEvents::eMOVE_FORWARD, Core::EEvents::eMOVE_RIGHT))
        {
            CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
            _transform_Component.tVertex += Normalize(temp_Vector + _view_Component.Front_Camera) * _camera_Speed;
            return true;
        }
        if(CompareDirection(_input_Stack, Core::EEvents::eMOVE_FORWARD, Core::EEvents::eMOVE_LEFT))
        {
            CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
            _transform_Component.tVertex += Normalize(_view_Component.Front_Camera - temp_Vector) * _camera_Speed;
            return true;
        }
        if(CompareDirection(_input_Stack, Core::EEvents::eMOVE_BACKWARD, Core::EEvents::eMOVE_LEFT))
        {
            CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
            _transform_Component.tVertex -= Normalize(_view_Component.Front_Camera + temp_Vector) * _camera_Speed;
            return true;
        }
        return false;
    }
}
