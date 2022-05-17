#ifndef MOVEMENT_SYSTEM
#define MOVEMENT_SYSTEM

#include "Event.hpp"
#include "TransformComponent.hpp"
#include "VectorContainer.hpp"
#include "MoveComponent.hpp"
#include "ComponentManager.hpp"
#include "ISystem.hpp"
#include "VertexMath.hpp"
#include "ViewComponent.hpp"
#include "Stack.hpp"

namespace GLVM::ECS
{
	class CMovementSystem : public ISystem
	{
	public:
		float _dOffset;
		Core::EEvents _Anim_Event;
        Core::CStack& Input_Stack_;
        Vector<float, 3> front{1.0f};

        CMovementSystem(Core::CStack& _input_Stack) : Input_Stack_(_input_Stack) {}
        
		void Update(ECS::CComponentManager& _Component_Manager, Core::CEvent& _Event) override
		{
            Core::TCVectorContainer<unsigned int>* pEntity_Container_refMove =
                ECS::GetInnerIDsContainer<ECS::SMoveComponent>(_Component_Manager);
            unsigned int u_iVector_Move_Size = pEntity_Container_refMove->GetSize();

            Core::TCVectorContainer<unsigned int>* pEntity_Container_refView =
                ECS::GetInnerIDsContainer<ECS::CViewComponent>(_Component_Manager);
            unsigned int u_iVector_View_Size = pEntity_Container_refView->GetSize();
            unsigned int iEntity_refView = (*pEntity_Container_refView)[0];
            ECS::CViewComponent& view_Component = _Component_Manager.GetComponent<ECS::CViewComponent>(iEntity_refView);
            
			for(int i = 0; i < u_iVector_Move_Size; ++i)
			{
                float cameraSpeed = 2.5f * _dOffset;            
                for(int n = 0, u_iNumber_of_Events = 5; n < u_iNumber_of_Events; ++n)
                {
                    bool bDiagonal_Movement_Availability = false;
                    unsigned int iEntity_refMove = (*pEntity_Container_refMove)[i];
                    bDiagonal_Movement_Availability = FixDiagonalMove(Input_Stack_,
                                           _Component_Manager.GetComponent<ECS::STransformComponent>(iEntity_refMove),
                                           cameraSpeed,
                                           view_Component,
                                           _Event);
                    if(bDiagonal_Movement_Availability)
                        break;
                    switch(Input_Stack_[n])
                    {
                    case Core::EEvents::eMOVE_LEFT:
                        _Component_Manager.GetComponent<ECS::STransformComponent>(iEntity_refMove).tVertex +=
                            CalculateVectorRL(_Component_Manager.GetComponent<ECS::STransformComponent>(iEntity_refMove),
                                              view_Component,
                                              _Component_Manager.GetComponent<ECS::SMoveComponent>(iEntity_refMove),
                                              cameraSpeed,
                                              Input_Stack_[n]);
                        break;
                    case Core::EEvents::eMOVE_RIGHT:
                        _Component_Manager.GetComponent<ECS::STransformComponent>(iEntity_refMove).tVertex +=
                            CalculateVectorRL(_Component_Manager.GetComponent<ECS::STransformComponent>(iEntity_refMove),
                                              view_Component,
                                              _Component_Manager.GetComponent<ECS::SMoveComponent>(iEntity_refMove),
                                              cameraSpeed,
                                              Input_Stack_[n]);
                        break;
                    case Core::EEvents::eMOVE_BACKWARD:
                        _Component_Manager.GetComponent<ECS::STransformComponent>(iEntity_refMove).tVertex +=
                            CalculateVectorFB(_Component_Manager.GetComponent<ECS::STransformComponent>(iEntity_refMove),
                                              _Component_Manager.GetComponent<ECS::SMoveComponent>(iEntity_refMove),
                                              cameraSpeed,
                                              view_Component,
                                              _Event,
                                              Input_Stack_[n]);
                        break;
                    case Core::EEvents::eMOVE_FORWARD:
                        _Component_Manager.GetComponent<ECS::STransformComponent>(iEntity_refMove).tVertex +=
                            CalculateVectorFB(_Component_Manager.GetComponent<ECS::STransformComponent>(iEntity_refMove),
                                              _Component_Manager.GetComponent<ECS::SMoveComponent>(iEntity_refMove),
                                              cameraSpeed,
                                              view_Component,
                                              _Event,
                                              Input_Stack_[n]);
                        break;
                    case Core::EEvents::eJUMP:
                        _Component_Manager.GetComponent<ECS::STransformComponent>(iEntity_refMove).tVertex[1] += 1.0f;
                        _Component_Manager.GetComponent<ECS::SMoveComponent>(iEntity_refMove).eEvent_ = Core::EEvents::eJUMP;
                        break;
                    default:
                        break;
                    }
                }
			}
		}

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
        
        void DiscardOpposite(Core::CStack& _input_Stack)
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

        Vector<float, 3> CalculateVectorRL(STransformComponent _transform_Component,
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

        Vector<float, 3> CalculateVectorFB(STransformComponent _transform_Component,
                                           SMoveComponent& _move_Component,
                                           float _camera_Speed,
                                           ECS::CViewComponent& _view_Component,
                                           Core::CEvent& _event,
                                           Core::EEvents _current_Event)
        {
            Vector<float, 3> front(0.0f);
            Vector<float, 3> temp_Vector(0.0f);
            _view_Component.Front_Camera[1] = 0.0f;
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

        void CalculatePerdendicularVectors(float _camera_Speed,
                                           ECS::CViewComponent& _view_Component,
                                           Core::CEvent& _event,
                                           Vector<float, 3>& _temp_Vector)
        {
            _view_Component.Front_Camera[1] = 0.0f;
            front[0] = std::cos(Radians(_event.mouse_Pointer_Position_.fYaw_));
            front[2] = std::sin(Radians(_event.mouse_Pointer_Position_.fYaw_));
            _view_Component.Front_Camera = Normalize(front) * _camera_Speed;
            _temp_Vector = Normalize(Cross(_view_Component.Front_Camera, _view_Component.Up_Camera)) * _camera_Speed;
        }
        
        bool FixDiagonalMove(Core::CStack& _input_Stack,
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
        
	};
}

#endif

