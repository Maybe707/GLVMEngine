#ifndef MOVEMENT_SYSTEM
#define MOVEMENT_SYSTEM

#include "Event.hpp"
#include "TransformComponent.hpp"
#include "VectorContainer.hpp"
#include "MoveComponent.hpp"
#include "ComponentManager.hpp"
#include "ISystem.hpp"

namespace GLVM::ECS
{
	class CMovementSystem : public ISystem
	{
	public:
		double _dOffset;
		Core::EEvents _Anim_Event;
		void Update(ECS::CComponentManager& _Component_Manager, Core::CEvent& _Event) override
		{
			Core::TCVectorContainer<STransformComponent>* _pTransform_Components_Container = ECS::GetInnerComponentContainer<ECS::STransformComponent>(_Component_Manager);
			Core::TCVectorContainer<unsigned int>* _pOrdered_Move_Container = ECS::GetInnerIDsContainer<ECS::SMoveComponent>(_Component_Manager);
			Core::TCVectorContainer<SMoveComponent>* _pMove_Components_Container = ECS::GetInnerComponentContainer<ECS::SMoveComponent>(_Component_Manager);
			for(int i = 0; i < _pOrdered_Move_Container->GetSize(); ++i)
			{
				switch(_Anim_Event)
				{
				case Core::EEvents::eMOVE_LEFT:
					(*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].fPos_X -= _dOffset;
					(*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eMOVE_LEFT;
					break;
				case Core::EEvents::eMOVE_RIGHT:
					(*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].fPos_X += _dOffset;
					(*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eMOVE_RIGHT;
					break;
				case Core::EEvents::eMOVE_DOWN:
					(*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].fPos_Y -= _dOffset;
					(*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eMOVE_DOWN; 
					break;
				case Core::EEvents::eMOVE_UP:
					(*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].fPos_Y += _dOffset;
					(*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eMOVE_UP;
					break;
				default:
					break;
				}
				
			}
		}
	};
}

#endif
