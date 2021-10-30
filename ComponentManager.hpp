#ifndef COMPONENT_MANAGER
#define COMPONENT_MANAGER

#include "IComponent.hpp"
#include "ConstVectorContainer.hpp"

#include "IContainer.hpp"

namespace GLVM::ECS
{
	class CComponentManager
	{
	public:
		inline static unsigned int s_iComponents_Container_ID = 0;
		Core::TCConstVectorContainer<Core::IContainer*> tMain_Container_;
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
			++s_iComponents_Container_ID;
			return s_iLocal_ID;
		}
		
		template <typename S>
		void BindComponentToEntity(unsigned int& _u_iEntity, S& _Component)
		{
			unsigned int u_iIndex;
			u_iIndex = CreateComponentContainer<S>();
			AddComponentToContainer<S>(_Component, u_iIndex, _u_iEntity);
		}
		
		template <typename S>
		void AddComponentToContainer(S& _Component, unsigned int _u_iIndex,
			unsigned int _u_iEntity)
		{
			static_cast<Core::TCConstVectorContainer<S>*>(tMain_Container_[_u_iIndex])->Push(_Component, _u_iEntity);
			
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
			}
		}
	};
}

#endif
