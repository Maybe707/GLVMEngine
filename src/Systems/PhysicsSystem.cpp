#include "Systems/PhysicsSystem.hpp"
#include "ComponentManager.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Event.hpp"
#include "Globals.hpp"

namespace GLVM::ECS
{
    bool CompareDirection(Core::CStack& _input_Stack,
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
    
    void CalculatePerdendicularVectors(float _camera_Speed,
                                       ECS::CViewComponent& _view_Component,
                                       Core::CEvent& _event,
                                       Vector<float, 3>& _temp_Vector)
    {
        Vector<float, 3> front{1.0f};
//        _view_Component.Front_Camera[1] = 0.0f;
        front[0] = std::cos(Radians(_event.mouse_Pointer_Position_.fYaw_));
        front[2] = std::sin(Radians(_event.mouse_Pointer_Position_.fYaw_));
        _view_Component.Front_Camera = Normalize(front) * _camera_Speed;
        _temp_Vector = Normalize(Cross(_view_Component.Front_Camera, _view_Component.Up_Camera)) * _camera_Speed;
    }
        
    bool FixDiagonalMoveReverse(Core::CStack& _input_Stack,
                                STransformComponent& _transform_Component,
                                float _camera_Speed,
                                ECS::CViewComponent& _view_Component,
                                Core::CEvent& _event)
    {
        Vector<float, 3> temp_Vector(0.0f);
        if(CompareDirection(_input_Stack, Core::EEvents::eMOVE_BACKWARD, Core::EEvents::eMOVE_RIGHT))
        {
            CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
            _transform_Component.tPosition += Normalize(_view_Component.Front_Camera - temp_Vector) * _camera_Speed;
            return true;
        }
        if(CompareDirection(_input_Stack, Core::EEvents::eMOVE_FORWARD, Core::EEvents::eMOVE_RIGHT))
        {
            CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
            _transform_Component.tPosition -= Normalize(temp_Vector + _view_Component.Front_Camera) * _camera_Speed;
            return true;
        }
        if(CompareDirection(_input_Stack, Core::EEvents::eMOVE_FORWARD, Core::EEvents::eMOVE_LEFT))
        {
            CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
            _transform_Component.tPosition -= Normalize(_view_Component.Front_Camera - temp_Vector) * _camera_Speed;
            return true;
        }
        if(CompareDirection(_input_Stack, Core::EEvents::eMOVE_BACKWARD, Core::EEvents::eMOVE_LEFT))
        {
            CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
            _transform_Component.tPosition += Normalize(_view_Component.Front_Camera + temp_Vector) * _camera_Speed;
            return true;
        }
        return false;
    }
    
    void CPhysicsSystem::Repel(STransformComponent& _transform_Component,
                                 SMoveComponent& _move_Component,
                                 float& _fDelta_Time,
                                 CViewComponent& _view_Component,
                                 Core::CEvent& _event)
    {
        for(int n = 0; n < 5; ++n)
        {
            float cameraSpeed = static_cast<float>(2.5 * _fDelta_Time);
            bool bDiagonal_Movement_Availability = false;
            ///< Check diagonal movement.
            bDiagonal_Movement_Availability = FixDiagonalMoveReverse(Input_Stack_,
                                                                     _transform_Component,
                                                                     cameraSpeed,
                                                                     _view_Component,
                                                                     _event);
            if(bDiagonal_Movement_Availability)
                break;
            switch(Input_Stack_[n])
            {
            case Core::eMOVE_FORWARD:
                _transform_Component.tPosition -= _transform_Component.tForward * cameraSpeed;
                break;
            case Core::eMOVE_BACKWARD:
                _transform_Component.tPosition += _transform_Component.tForward * cameraSpeed;
                break;
            case Core::eMOVE_RIGHT:
                _transform_Component.tPosition -= _transform_Component.tRight * cameraSpeed;
                break;
            case Core::eMOVE_LEFT:
                _transform_Component.tPosition += _transform_Component.tRight * cameraSpeed;
                break;
            default:
                break;
            }
        }
    }

    ///< Set Y-axis of transform component of backtracking entity to upper Y-axis of ground entity.
        
    void CPhysicsSystem::Gravity(STransformComponent& _transform_Component1, STransformComponent& _transform_Component2)
    {
        // _transform_Component1.tPosition[1] = _transform_Component2.tPosition[1] + _transform_Component2.fScale / 2 + _transform_Component1.fScale / 2;
        // fAcceleration_of_Gravity_ = 0.0f;
        CComponentManager* pComponent_Manager = CComponentManager::GetInstance();
        
        float cameraSpeed = 2.5f * fDelta_Time_;            
        
        for(int n = 0; n < ECS::GetInnerIDsContainer<ECS::CRigidBodyComponent>(*pComponent_Manager)->GetSize(); ++n)
        {
            int iEntity_refRigidBody = (*ECS::GetInnerIDsContainer<ECS::CRigidBodyComponent>(*pComponent_Manager))[n];
            //    pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refRigidBody).tPosition[1] -= fAcceleration_of_Gravity_;
            ECS::STransformComponent& rTransform_Component = pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refRigidBody);
            Vector<float, 3> vec(0.0f);
            vec[1] = -1.0f;
            rTransform_Component.tPosition -= vec * cameraSpeed;
        }
    }

    /*! This update searching for refering to colliders entities and check their
     *  transform components for collision, and if collision detected check if
     *  backtracking entity had gravity component for call Gravity function.
     */
         
    void CPhysicsSystem::Update() 
    {
        CComponentManager* pComponent_Manager = CComponentManager::GetInstance();
        Core::TCVectorContainer<unsigned int>* pEntity_Container_refCollider =
            ECS::GetInnerIDsContainer<ECS::CColliderComponent>(*pComponent_Manager);
        unsigned int uiVector_Collider_Size = pEntity_Container_refCollider->GetSize();
        Core::TCVectorContainer<unsigned int>* pEntity_Container_refRigidBody =
            ECS::GetInnerIDsContainer<ECS::CRigidBodyComponent>(*pComponent_Manager);
        unsigned int uiVector_RigidBody_Size = pEntity_Container_refRigidBody->GetSize();
            
        for(int i = 0, iSize_External = uiVector_Collider_Size; i < iSize_External; ++i)
        {
                unsigned int uiEntity_refCollider = (*pEntity_Container_refCollider)[i];
                
                if(pComponent_Manager->GetComponent<ECS::CColliderComponent>(uiEntity_refCollider).bGround_Collision_)
                {
                    unsigned int uiCollider = pComponent_Manager->GetComponent<ECS::CColliderComponent>(uiEntity_refCollider).uiGround_Collider_;
                    Gravity(pComponent_Manager->GetComponent<ECS::STransformComponent>(uiEntity_refCollider),
                            pComponent_Manager->GetComponent<ECS::STransformComponent>(uiCollider));
                    pComponent_Manager->GetComponent<ECS::CColliderComponent>(uiEntity_refCollider).bGround_Collision_ = false;
                }
                if(pComponent_Manager->GetComponent<ECS::CColliderComponent>(uiEntity_refCollider).bWall_Collision_
                    && pComponent_Manager->GetComponent<ECS::CColliderComponent>(uiEntity_refCollider).bPush_Collission)
                {
                    Repel(pComponent_Manager->GetComponent<ECS::STransformComponent>(uiEntity_refCollider),
                          pComponent_Manager->GetComponent<ECS::SMoveComponent>(uiEntity_refCollider),
                          fDelta_Time_,
                          pComponent_Manager->GetComponent<ECS::CViewComponent>(uiEntity_refCollider),
                          g_eEvent);
                    pComponent_Manager->GetComponent<ECS::CColliderComponent>(uiEntity_refCollider).bWall_Collision_ = false;
                }
        }

        ///< Forcing all entities that had gravity component falling down.

        // for(int n = 0; n < ECS::GetInnerIDsContainer<ECS::CRigidBodyComponent>(*pComponent_Manager)->GetSize(); ++n)
        // {
        //     int iEntity_refRigidBody = (*ECS::GetInnerIDsContainer<ECS::CRigidBodyComponent>(*pComponent_Manager))[n];
        //     pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refRigidBody).tPosition[1] -= fAcceleration_of_Gravity_;
        // }


    }
}
