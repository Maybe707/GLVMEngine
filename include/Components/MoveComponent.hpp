#ifndef MOVE_COMPONENT
#define MOVE_COMPONENT

#include "Event.hpp"
namespace GLVM::ECS
{
	struct SMoveComponent
	{
		Core::EEvents eEvent_ = Core::EEvents::eDEFAULT;
	};
}

#endif
