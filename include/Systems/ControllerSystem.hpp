#ifndef CONTROLLER_SYSTEM
#define CONTROLLER_SYSTEM

#include "Event.hpp"
#include "Stack.hpp"

namespace GLVM::core
{
	class ControllerSystem
	{
		CStack& inputStack_;
		CEvent event_;
	public:
		ControllerSystem(CStack& inputStack, CEvent& event);
		void Update();
	};
}

#endif
