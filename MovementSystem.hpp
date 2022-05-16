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

        CMovementSystem(Core::CStack& _input_Stack) : Input_Stack_(_input_Stack) {}
        
		void Update(ECS::CComponentManager& _Component_Manager, Core::CEvent& _Event) override
		{
			Core::TCVectorContainer<STransformComponent>* _pTransform_Components_Container = ECS::GetInnerComponentContainer<ECS::STransformComponent>(_Component_Manager);
			Core::TCVectorContainer<unsigned int>* _pOrdered_Move_Container = ECS::GetInnerIDsContainer<ECS::SMoveComponent>(_Component_Manager);
			Core::TCVectorContainer<SMoveComponent>* _pMove_Components_Container = ECS::GetInnerComponentContainer<ECS::SMoveComponent>(_Component_Manager);
            Core::TCVectorContainer<ECS::CViewComponent>* _tViewContainer = ECS::GetInnerComponentContainer<ECS::CViewComponent>(_Component_Manager);
            Core::TCVectorContainer<unsigned int>* _pOrdered_View_Container = ECS::GetInnerIDsContainer<ECS::CViewComponent>(_Component_Manager);
            ECS::CViewComponent& view_Component = (*_tViewContainer)[(*_pOrdered_View_Container)[0]];  //!!!!!!!! REMOVE HARDCODE !!!!!!!!!!!
			for(int i = 0; i < _pOrdered_Move_Container->GetSize(); ++i)
			{
                float cameraSpeed = 1.5f * _dOffset;
                Vector<float, 3> front(1.0f);
                Vector<float, 3> temp_Vector0(0.0f);
                Vector<float, 3> temp_Vector1(0.0f);
                Vector<float, 3> temp_Vector2(0.0f);
                Vector<float, 3> temp_Vector3(0.0f);
                if(CompareDirection(Input_Stack_, Core::EEvents::eMOVE_FORWARD, Core::EEvents::eMOVE_RIGHT))
                {
                    std::cout << "Privet" << std::endl;
                    Input_Stack_.Remove(Core::EEvents::eMOVE_FORWARD);
                    Input_Stack_.Remove(Core::EEvents::eMOVE_BACKWARD);
                    Input_Stack_.Push(Core::EEvents::eMOVE_DIAGONAL);
                }
                for(int n = 0; n < 5; ++n)
                {
                    switch((Input_Stack_)[n])
                    {
                    case Core::EEvents::eMOVE_LEFT:
                        // (*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].tVertex -=
                        //     Normalize(Cross(view_Component.Front_Camera, view_Component.Up_Camera)) * cameraSpeed;
                        // (*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eMOVE_LEFT;
                        temp_Vector0 = CalculateVectorRL((*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]],
                                                         view_Component,
                                                         (*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]],
                                                         cameraSpeed,
                                                         Input_Stack_[n]);
                        (*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].tVertex += temp_Vector0;
                        break;
                    case Core::EEvents::eMOVE_RIGHT:
                        // (*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].tVertex +=
                        //     Normalize(Cross(view_Component.Front_Camera, view_Component.Up_Camera)) * cameraSpeed;
                        // (*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eMOVE_RIGHT;
                        temp_Vector1 = CalculateVectorRL((*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]],
                                                         view_Component,
                                                         (*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]],
                                                         cameraSpeed,
                                                         Input_Stack_[n]);
                        (*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].tVertex += temp_Vector1;
                        break;
                    case Core::EEvents::eMOVE_BACKWARD:
                        // view_Component.Front_Camera[1] = 0.0f;
                        // front[0] = std::cos(Radians(_Event.mouse_Pointer_Position_.fYaw_));
                        // front[2] = std::sin(Radians(_Event.mouse_Pointer_Position_.fYaw_));
                        // view_Component.Front_Camera = Normalize(front);
                        // (*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].tVertex -=
                        //     view_Component.Front_Camera * cameraSpeed;
                        // (*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eMOVE_BACKWARD;
                        temp_Vector2 = CalculateVectorFB((*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]],
                                                         (*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]],
                                                         cameraSpeed,
                                                         view_Component,
                                                         _Event,
                                                         Input_Stack_[n]);
                        (*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].tVertex += temp_Vector2;
                        break;
                    case Core::EEvents::eMOVE_FORWARD:
                        // view_Component.Front_Camera[1] = 0.0f;
                        // front[0] = std::cos(Radians(_Event.mouse_Pointer_Position_.fYaw_));
                        // front[2] = std::sin(Radians(_Event.mouse_Pointer_Position_.fYaw_));
                        // view_Component.Front_Camera = Normalize(front);
                        // (*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].tVertex +=
                        //     view_Component.Front_Camera * cameraSpeed;
                        // (*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eMOVE_FORWARD;
                        temp_Vector3 = CalculateVectorFB((*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]],
                                                         (*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]],
                                                         cameraSpeed,
                                                         view_Component,
                                                         _Event,
                                                         Input_Stack_[n]);
                        (*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].tVertex += temp_Vector3;
                        break;
                    case Core::EEvents::eMOVE_DIAGONAL:
                        view_Component.Front_Camera[1] = 0.0f;
                        front[0] = std::cos(Radians(_Event.mouse_Pointer_Position_.fYaw_));
                        front[2] = std::sin(Radians(_Event.mouse_Pointer_Position_.fYaw_));
                        view_Component.Front_Camera = Normalize(front) * cameraSpeed;
                        temp_Vector0 = Normalize(Cross(view_Component.Front_Camera, view_Component.Up_Camera)) * cameraSpeed;
                        temp_Vector0 = temp_Vector0 + view_Component.Front_Camera;
                        temp_Vector0 = Normalize(temp_Vector0);
                        (*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].tVertex += Normalize(temp_Vector0) * cameraSpeed;
                        break;
                    case Core::EEvents::eJUMP:
                        (*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].tVertex[1] += 1.0f;
                        (*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eJUMP;
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
        
	};
}

#endif

// case Core::EEvents::eMOVE_DOWN:
// 	(*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].tVertex -=
//         view_Component.Front_Camera * cameraSpeed;
// 	(*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eMOVE_DOWN;
// 	break;
// case Core::EEvents::eMOVE_UP:
// 	(*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].tVertex +=
//         view_Component.Front_Camera * cameraSpeed;
// 	(*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eMOVE_UP;
// 	break;
