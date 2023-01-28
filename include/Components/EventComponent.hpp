#ifndef EVENT_COMPONENT
#define EVENT_COMPONENT

#include "../Event.hpp"

namespace GLVM::ecs::components
{
	struct event
	{
		core::EEvents eEvent_;
	};
}

#endif
