#include "ComponentManager.hpp"

namespace GLVM::ECS
{
	void CComponentManager::CreateComponentContainer(IComponent* _Component)
	{
		_Component->u_iComponent_ID = s_aComponents_Container_ID;
		
		++s_aComponents_Container_ID;
		
	}
}
