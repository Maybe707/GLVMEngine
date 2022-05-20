#include "Systems/CollisionSystem.hpp"
#include "ComponentManager.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Event.hpp"
#include "Components/EventComponent.hpp"
#include "Components/GravityComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Stack.hpp"
#include "VectorContainer.hpp"

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
        _view_Component.Front_Camera[1] = 0.0f;
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
            _transform_Component.tVertex += Normalize(_view_Component.Front_Camera - temp_Vector) * _camera_Speed;
            return true;
        }
        if(CompareDirection(_input_Stack, Core::EEvents::eMOVE_FORWARD, Core::EEvents::eMOVE_RIGHT))
        {
            CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
            _transform_Component.tVertex -= Normalize(temp_Vector + _view_Component.Front_Camera) * _camera_Speed;
            return true;
        }
        if(CompareDirection(_input_Stack, Core::EEvents::eMOVE_FORWARD, Core::EEvents::eMOVE_LEFT))
        {
            CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
            _transform_Component.tVertex -= Normalize(_view_Component.Front_Camera - temp_Vector) * _camera_Speed;
            return true;
        }
        if(CompareDirection(_input_Stack, Core::EEvents::eMOVE_BACKWARD, Core::EEvents::eMOVE_LEFT))
        {
            CalculatePerdendicularVectors(_camera_Speed, _view_Component, _event, temp_Vector);
            _transform_Component.tVertex += Normalize(_view_Component.Front_Camera + temp_Vector) * _camera_Speed;
            return true;
        }
        return false;
    }
    
	void CCollisionSystem::Repel(STransformComponent& _transform_Component,
                                 SMoveComponent& _move_Component,
                                 float& _fDelta_Time,
                                 CViewComponent& _view_Component,
                                 Core::CEvent& _event)
	{
        // switch(_move_Component.eEvent_)
        // {
        // case Core::eMOVE_UP:
        //     _transform_Component.tVertex[2] = (_transform_Component.tVertex[2] - _fDelta_Time);
        // case Core::eMOVE_DOWN:
        //     _transform_Component.tVertex[2] = (_transform_Component.tVertex[2] + _fDelta_Time);
        // case Core::eMOVE_RIGHT:
        //     _transform_Component.tVertex[0] = (_transform_Component.tVertex[0] - _fDelta_Time);
        // case Core::eMOVE_LEFT:
        //     _transform_Component.tVertex[0] = (_transform_Component.tVertex[0] + _fDelta_Time);
        // default:
        //     break;
        // }
        
        // if(_event_Component.eEvent_ == Core::eGRAVITY_COLLISION_FLAG)
        // {
        //     _transform_Component.tVertex[1] += 0.01f;
        //     return;
        // }
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
                _transform_Component.tVertex -= _view_Component.Front_Camera * cameraSpeed;
                break;
            case Core::eMOVE_BACKWARD:
                _transform_Component.tVertex += _view_Component.Front_Camera * cameraSpeed;
                break;
            case Core::eMOVE_RIGHT:
                _transform_Component.tVertex -= Normalize(Cross(_view_Component.Front_Camera, _view_Component.Up_Camera)) * cameraSpeed;
                break;
            case Core::eMOVE_LEFT:
                _transform_Component.tVertex += Normalize(Cross(_view_Component.Front_Camera, _view_Component.Up_Camera)) * cameraSpeed;
                break;
            default:
                break;
            }
        }
	}

    bool CCollisionSystem::Gravity(STransformComponent& _transform_Component, SEventComponent& _event_Component)
    {
        if(_event_Component.eEvent_ == Core::eGRAVITY_COLLISION_FLAG)
        {
            _transform_Component.tVertex[1] += 0.01f;
            return false;
        }
        
        return true;
    }
    
	bool CCollisionSystem::BoxCollider(STransformComponent& _transform_Component1,
                                       STransformComponent& _transform_Component2,
                                       SEventComponent& _event_Component,
                                       float& _fDelta_Time)
	{
		bool bCollision_Flag = false;

//        if(_transform_Component1.tVertex[1] > (_transform_Component2.tVertex[1] + 0.45f))
        if((_transform_Component1.tVertex[1] - _transform_Component1.fScale / 2) > (_transform_Component2.tVertex[1] + (_transform_Component2.fScale / 2  - 0.1f)))
        {
            bCollision_Flag = false;
            return bCollision_Flag;
        }

        // std::cout << "Scale one: " << _transform_Component1.fScale << std::endl;
        // std::cout << "Scale two: " << _transform_Component2.fScale << std::endl;
        
        // if((_transform_Component1.tVertex[0] + _transform_Component1.fScale) > _transform_Component2.tVertex[0] &&
        //    _transform_Component1.tVertex[0] < _transform_Component2.tVertex[0] + _transform_Component2.fScale   &&
        //    (_transform_Component1.tVertex[1] + _transform_Component1.fScale) > _transform_Component2.tVertex[1] &&
        //    _transform_Component1.tVertex[1] < _transform_Component2.tVertex[1] + _transform_Component2.fScale   &&
        //    (_transform_Component1.tVertex[2] + _transform_Component1.fScale) > _transform_Component2.tVertex[2] &&
        //    _transform_Component1.tVertex[2] < _transform_Component2.tVertex[2] + _transform_Component2.fScale)
            
        // if((_transform_Component1.tVertex[0] + 0.5f) > _transform_Component2.tVertex[0] &&
        //    _transform_Component1.tVertex[0] < _transform_Component2.tVertex[0] + 0.5f   &&
        //    (_transform_Component1.tVertex[1] + 0.5f) > _transform_Component2.tVertex[1] &&
        //    _transform_Component1.tVertex[1] < _transform_Component2.tVertex[1] + 0.5f   &&
        //    (_transform_Component1.tVertex[2] + 0.5f) > _transform_Component2.tVertex[2] &&
        //    _transform_Component1.tVertex[2] < _transform_Component2.tVertex[2] + 0.5f)
        if(_transform_Component1.tVertex[0] + _transform_Component1.fScale / 2 > _transform_Component2.tVertex[0] - _transform_Component2.fScale / 2 &&
           _transform_Component1.tVertex[0] - _transform_Component1.fScale / 2 < _transform_Component2.tVertex[0] + _transform_Component2.fScale / 2 &&
           _transform_Component1.tVertex[1] + _transform_Component1.fScale / 2 > _transform_Component2.tVertex[1] - _transform_Component2.fScale / 2 &&
           _transform_Component1.tVertex[1] - _transform_Component1.fScale / 2 < _transform_Component2.tVertex[1] + _transform_Component2.fScale / 2 &&
           _transform_Component1.tVertex[2] + _transform_Component1.fScale / 2 > _transform_Component2.tVertex[2] - _transform_Component2.fScale / 2 &&
           _transform_Component1.tVertex[2] - _transform_Component1.fScale / 2 < _transform_Component2.tVertex[2] + _transform_Component2.fScale / 2)
        {
            bCollision_Flag = true;
        }
		return bCollision_Flag;
	}

	void CCollisionSystem::Update(CComponentManager& _Component_Manager, Core::CEvent& _Event)
	{
        Core::TCVectorContainer<unsigned int>* pEntity_Container_refCollider =
            ECS::GetInnerIDsContainer<ECS::CColliderComponent>(_Component_Manager);
        unsigned int uiVector_Collider_Size = pEntity_Container_refCollider->GetSize();
        Core::TCVectorContainer<unsigned int>* pEntity_Container_refEvent =
            ECS::GetInnerIDsContainer<ECS::SEventComponent>(_Component_Manager);
        Core::TCVectorContainer<unsigned int>* pEntity_Container_refMove =
            ECS::GetInnerIDsContainer<ECS::SMoveComponent>(_Component_Manager);
        unsigned int uiVector_Move_Size = pEntity_Container_refMove->GetSize();
        Core::TCVectorContainer<unsigned int>* pEntity_Container_refView =
            ECS::GetInnerIDsContainer<ECS::CViewComponent>(_Component_Manager);
        
		for(int i = 0, iSize = uiVector_Collider_Size; i < iSize; ++i)
		{
			for(int j = 0, iSize_Iner = uiVector_Collider_Size; j < iSize_Iner; ++j)
			{
                unsigned int uiBacktracking_Entity_refCollider = (*pEntity_Container_refCollider)[i];
                unsigned int uiCompared_Entity_refCollider = (*pEntity_Container_refCollider)[j];
                unsigned int uiEntity_refEvent = (*pEntity_Container_refEvent)[i];

                // std::cout << "Container 1 size: " << iSize << std::endl;
                // std::cout << "Container 2 size: " << iSize_Iner << std::endl;
                // std::cout << "Index 1: " << i << std::endl;
                // std::cout << "Index 2: " << j << std::endl;
                // std::cout << "Entity 1: " << uiBacktracking_Entity_refCollider << std::endl;
                // std::cout << "Entity 2: " << uiCompared_Entity_refCollider << std::endl;
                
				if(uiBacktracking_Entity_refCollider == uiCompared_Entity_refCollider)
					continue;
				if(BoxCollider(_Component_Manager.GetComponent<ECS::STransformComponent>(uiBacktracking_Entity_refCollider),
                               _Component_Manager.GetComponent<ECS::STransformComponent>(uiCompared_Entity_refCollider),
                               _Component_Manager.GetComponent<ECS::SEventComponent>(uiEntity_refEvent), fDelta_Time_))
					for(int x = 0, iSize_Rep = uiVector_Move_Size; x < iSize_Rep; ++x)
                    {
                        unsigned int uiEntity_refMove = (*pEntity_Container_refMove)[x];
                        unsigned int uiEntity_refView = (*pEntity_Container_refView)[x];
                            
                        if(uiBacktracking_Entity_refCollider == uiEntity_refMove &&
                           uiBacktracking_Entity_refCollider == uiEntity_refView)
                        {
							Repel(_Component_Manager.GetComponent<ECS::STransformComponent>(uiEntity_refMove),
                                  _Component_Manager.GetComponent<ECS::SMoveComponent>(uiEntity_refMove),
                                  fDelta_Time_,
                                  _Component_Manager.GetComponent<ECS::CViewComponent>(uiEntity_refView),
                                _Event);
                        }
                    }
			}
		}
	}

}
