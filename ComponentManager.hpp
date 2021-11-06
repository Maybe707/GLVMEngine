#ifndef COMPONENT_MANAGER
#define COMPONENT_MANAGER

#include "ConstVectorContainer.hpp"
#include "VectorContainer.hpp"
#include <iostream>
#include "IContainer.hpp"

namespace GLVM::ECS
{
	class CComponentManager
	{
	public:
		inline static unsigned int s_iComponents_Container_ID = 0;
		Core::TCConstVectorContainer<Core::IContainer*> tMain_Container_;
		Core::TCConstVectorContainer<Core::IContainer*> tOrdered_Container_;
	public:
		template <typename S>
		unsigned int CreateComponentContainer()
		{
			static unsigned int s_iLocal_ID = 0;
			static bool s_bComponent_Container_Flag = false;
			if(s_bComponent_Container_Flag)
				return s_iLocal_ID;
			s_iLocal_ID = s_iComponents_Container_ID;
			s_bComponent_Container_Flag = true;
			Core::TCConstVectorContainer<S>* pComponent_Container =
				new Core::TCConstVectorContainer<S>;
			tMain_Container_.Push(pComponent_Container, s_iComponents_Container_ID);
			Core::TCVectorContainer<unsigned int>* pOrdered_Indexes_Container =
				new Core::TCVectorContainer<unsigned int>;
			tOrdered_Container_.Push(pOrdered_Indexes_Container, s_iComponents_Container_ID);
			++s_iComponents_Container_ID;
			return s_iLocal_ID;
		}

		template <typename S>
		S& CreateComponent(unsigned int& _u_iEntity)
		{
			unsigned int u_iIndex; ///< Index for Main and Ordered containers.
			S Component;
			u_iIndex = CreateComponentContainer<S>();
			static_cast<Core::TCConstVectorContainer<S>*>(tMain_Container_[u_iIndex])->Push(Component, _u_iEntity);
			static_cast<Core::TCVectorContainer<unsigned int>*>(tOrdered_Container_[u_iIndex])->Push(_u_iEntity);
			return (*static_cast<Core::TCConstVectorContainer<S>*>(tMain_Container_[u_iIndex]))[_u_iEntity];
		}
		
		Core::TCConstVectorContainer<Core::IContainer*>& GetWorldContainer()
		{
			return tMain_Container_;
		}

		unsigned int GetContainerID()
		{
			return s_iComponents_Container_ID;
		}
		
		~CComponentManager()
		{
			for(int i = 0; i < s_iComponents_Container_ID; ++i)
			{
				delete tMain_Container_[i];
				tMain_Container_[i] = nullptr;
				delete tOrdered_Container_[i];
				tOrdered_Container_[i] = nullptr;
			}
		}
	};
}

#endif
