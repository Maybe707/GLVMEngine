#ifndef MOVEMENT_SYSTEM
#define MOVEMENT_SYSTEM

#include "Event.hpp"
#include "TransformComponent.hpp"

namespace GLVM::ECS
{
	class CMovementSystem
	{
	public:
		void Move(STransformComponent& _Transform_Component, double _dOffset, Core::EEvents _Event)
		{ 
			switch(_Event)
			{
			case Core::EEvents::eMOVE_LEFT:
				_Transform_Component.fPos_X -= _dOffset;
				break;
			case Core::EEvents::eMOVE_RIGHT:
				_Transform_Component.fPos_X += _dOffset;
				break;
			case Core::EEvents::eMOVE_DOWN:
				_Transform_Component.fPos_Y -= _dOffset;
				break;
			case Core::EEvents::eMOVE_UP:
				_Transform_Component.fPos_Y += _dOffset;
				break;
			default:
				break;
			}
		}
	};
}

#endif
