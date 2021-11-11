#ifndef ISYSTEM
#define ISYSTEM

#include "ComponentManager.hpp"

namespace GLVM::ECS
{
	class ISystem
	{
	public:
		virtual ~ISystem() {}
		virtual void Update(ECS::CComponentManager& _Component_Manager) = 0;
	};
}

#endif
