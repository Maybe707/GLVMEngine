#ifndef ISYSTEM
#define ISYSTEM

#include "ComponentManager.hpp"
#include "Event.hpp"

namespace GLVM::ECS
{
	class ISystem
	{
	public:
		virtual ~ISystem() {}
		virtual void Update(ECS::CComponentManager& _Component_Manager, Core::CEvent& _Event) = 0;
	};
}

#endif
