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
                Vector<float, 3> temp_vector(0.0f);
                if(CompareDirection(Input_Stack_, Core::EEvents::eMOVE_UP, Core::EEvents::eMOVE_RIGHT))
                {
                    std::cout << "Privet" << std::endl;
                    Input_Stack_.Remove(Core::EEvents::eMOVE_UP);
                    Input_Stack_.Remove(Core::EEvents::eMOVE_DOWN);
                    Input_Stack_.Push(Core::EEvents::eMOVE_DIAGONAL);
                }
                for(int n = 0; n < 5; ++n)
                {
                    switch((Input_Stack_)[n])
                    {
                    case Core::EEvents::eMOVE_LEFT:
                        (*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].tVertex -=
                            Normalize(Cross(view_Component.Front_Camera, view_Component.Up_Camera)) * cameraSpeed;
                        (*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eMOVE_LEFT;
                        break;
                    case Core::EEvents::eMOVE_RIGHT:
                        (*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].tVertex +=
                            Normalize(Cross(view_Component.Front_Camera, view_Component.Up_Camera)) * cameraSpeed;
                        (*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eMOVE_RIGHT;
                        break;
                    case Core::EEvents::eMOVE_DOWN:
                        view_Component.Front_Camera[1] = 0.0f;
                        front[0] = std::cos(Radians(_Event.mouse_Pointer_Position_.fYaw_));
                        front[2] = std::sin(Radians(_Event.mouse_Pointer_Position_.fYaw_));
                        view_Component.Front_Camera = Normalize(front);
                        (*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].tVertex -=
                            view_Component.Front_Camera * cameraSpeed;
                        (*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eMOVE_DOWN;
                        break;
                    case Core::EEvents::eMOVE_UP:
                        view_Component.Front_Camera[1] = 0.0f;
                        front[0] = std::cos(Radians(_Event.mouse_Pointer_Position_.fYaw_));
                        front[2] = std::sin(Radians(_Event.mouse_Pointer_Position_.fYaw_));
                        view_Component.Front_Camera = Normalize(front);
                        (*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].tVertex +=
                            view_Component.Front_Camera * cameraSpeed;
                        (*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eMOVE_UP;
                        break;
                    case Core::EEvents::eMOVE_DIAGONAL:
                        view_Component.Front_Camera[1] = 0.0f;
                        front[0] = std::cos(Radians(_Event.mouse_Pointer_Position_.fYaw_));
                        front[2] = std::sin(Radians(_Event.mouse_Pointer_Position_.fYaw_));
                        view_Component.Front_Camera = Normalize(front) * cameraSpeed;
                        temp_vector = Normalize(Cross(view_Component.Front_Camera, view_Component.Up_Camera)) * cameraSpeed;
                        temp_vector = temp_vector + view_Component.Front_Camera;
                        temp_vector = Normalize(temp_vector);
                        (*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].tVertex += Normalize(temp_vector) * cameraSpeed;
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
            if(CompareDirection(_input_Stack, Core::EEvents::eMOVE_DOWN, Core::EEvents::eMOVE_UP))
            {
                _input_Stack.Remove(Core::EEvents::eMOVE_DOWN);
                _input_Stack.Remove(Core::EEvents::eMOVE_UP);
            }
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
