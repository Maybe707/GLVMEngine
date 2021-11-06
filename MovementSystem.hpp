#ifndef MOVEMENT_SYSTEM
#define MOVEMENT_SYSTEM

#include "ConstVectorContainer.hpp"
#include "Event.hpp"
#include "TransformComponent.hpp"
#include "VectorContainer.hpp"
#include "MoveComponent.hpp"
#include <iostream>

namespace GLVM::ECS
{
	class CMovementSystem
	{
	public:
		void Move(Core::TCConstVectorContainer<STransformComponent>* _pTransform_Components_Container, Core::TCVectorContainer<unsigned int>* _pOrdered_Move_Container, Core::TCConstVectorContainer<SMoveComponent>* _pMove_Components_Container, double _dOffset, Core::EEvents _Event)
		{
			for(int i = 0; i < _pOrdered_Move_Container->GetSize(); ++i)
			{
				switch(_Event)
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
