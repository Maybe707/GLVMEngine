#ifndef SYSTEM_MANAGER
#define SYSTEM_MANAGER

#include "ComponentManager.hpp"

namespace GLVM::ECS
{
	class TCSystemManager
	{
	public:
		template<typename T>
		void Method(CComponentManager& _Component_Manager)
		{
			_Component_Manager.CreateComponentContainer<T>();
		}
	};
}

#endif
