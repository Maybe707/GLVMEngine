#ifndef MOVE_COMPONENT
#define MOVE_COMPONENT

#include "Event.hpp"
namespace GLVM::ecs
{
	struct move
	{
        float fVelocity_;
		core::EEvents eEvent_ = core::EEvents::eDEFAULT;
	};
}

#endif
