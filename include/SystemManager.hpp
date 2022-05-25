#ifndef SYSTEM_MANAGER
#define SYSTEM_MANAGER

#include "ISystem.hpp"
#include "VectorContainer.hpp"

namespace GLVM::ECS
{
	class CSystemManager : public ISystem
	{
	public:
		inline static unsigned int s_iSystem_ID = 0;
		Core::TCVectorContainer<ISystem*> tSystemContainer;

		void ActivateSystem(ISystem* _System)
		{
			tSystemContainer.Push(_System);
			++s_iSystem_ID;
		}

		void Update() override
		{
			for(int i = 0; i < s_iSystem_ID; ++i)
				tSystemContainer[i]->Update();
		}
	};
}

#endif
