#ifndef ISYSTEM
#define ISYSTEM

#include "ComponentManager.hpp"
#include "Event.hpp"

namespace GLVM::ecs
{
	class ISystem
	{
	public:
		virtual ~ISystem() {}
		virtual void Update() = 0;
	};
}

#endif
