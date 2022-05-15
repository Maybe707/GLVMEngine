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
                float cameraSpeed = 2.5f * _dOffset;
				switch(_Anim_Event)
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
				case Core::EEvents::eMOVE_DOWN:
                    view_Component.Front_Camera[1] = 0.0f;
					(*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].tVertex -=
                        view_Component.Front_Camera * cameraSpeed;
					(*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eMOVE_DOWN;
					break;
				case Core::EEvents::eMOVE_UP:
                    view_Component.Front_Camera[1] = 0.0f;
					(*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].tVertex +=
                        view_Component.Front_Camera * cameraSpeed;
					(*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eMOVE_UP;
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
	};
}

#endif
