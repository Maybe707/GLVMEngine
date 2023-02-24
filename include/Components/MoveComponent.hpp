#ifndef MOVE_COMPONENT
#define MOVE_COMPONENT

#include "VertexMath.hpp"
#include "Event.hpp"

namespace GLVM::ecs::components
{
	struct move
	{
        // float fVelocity_;
		core::EEvents eEvent_ = core::EEvents::eDEFAULT;
		vec3 frameMovement{ 0.0f, 0.0f, 0.0f };
		vec3 gravity{ 0.0f, 0.0f, 0.0f };
	};
}

#endif
