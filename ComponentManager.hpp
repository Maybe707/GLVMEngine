#ifndef COMPONENT_MANAGER
#define COMPONENT_MANAGER

#include "VectorContainer.hpp"
#include <iostream>
#include "IContainer.hpp"

namespace GLVM::ECS
{
	class CComponentManager
	{
	public:
		inline static unsigned int s_iComponents_Container_ID = 0;
		Core::TCVectorContainer<Core::IContainer*> tMain_Container_;
		Core::TCVectorContainer<Core::IContainer*> tOrdered_Container_;
		template <typename S>
		unsigned int CreateComponentContainer()
		{
			static unsigned int s_iLocal_ID = 0;
			static bool s_bComponent_Container_Flag = false;
			if(s_bComponent_Container_Flag)
				return s_iLocal_ID;
			s_iLocal_ID = s_iComponents_Container_ID;
			s_bComponent_Container_Flag = true;
			Core::TCVectorContainer<S>* pComponent_Container =
				new Core::TCVectorContainer<S>;
			tMain_Container_.Insert(pComponent_Container, s_iComponents_Container_ID);
			Core::TCVectorContainer<unsigned int>* pOrdered_Indexes_Container =
				new Core::TCVectorContainer<unsigned int>;
			tOrdered_Container_.Insert(pOrdered_Indexes_Container, s_iComponents_Container_ID);
			++s_iComponents_Container_ID;
			return s_iLocal_ID;
		}

		template <typename S>
		S& CreateComponent(unsigned int& _u_iEntity)
		{
			unsigned int u_iIndex; ///< Index for Main and Ordered containers.
			S Component;
			u_iIndex = CreateComponentContainer<S>();
			static_cast<Core::TCVectorContainer<S>*>(tMain_Container_[u_iIndex])->Insert(Component, _u_iEntity);
			static_cast<Core::TCVectorContainer<unsigned int>*>(tOrdered_Container_[u_iIndex])->Push(_u_iEntity);
			return (*static_cast<Core::TCVectorContainer<S>*>(tMain_Container_[u_iIndex]))[_u_iEntity];
		}

		template <typename S>
		void RemoveComponent(unsigned int& _u_iEntity)
		{
//			static_cast<Core::TCConstVectorContainer<S>*>(tMain_Container_[CreateComponentContainer<S>()])->Remove(_u_iEntity);
			static_cast<Core::TCVectorContainer<unsigned int>*>(tOrdered_Container_[CreateComponentContainer<S>()])->RemoveItem(_u_iEntity);
		}
		
		unsigned int GetContainerID()
		{
			return s_iComponents_Container_ID;
		}
		
		~CComponentManager()
		{
			for(int i = 0, iSize_Main = tMain_Container_.GetSize(); i < iSize_Main; ++i)
			{
				delete tMain_Container_[i];
				tMain_Container_[i] = nullptr;
			}
			for(int j = 0, iSize_Ordered = tOrdered_Container_.GetSize(); j < iSize_Ordered; ++j)
			{
				delete tOrdered_Container_[j];
				tOrdered_Container_[j] = nullptr;
			}
		}
	};

	template <typename T>
	Core::TCVectorContainer<T>* GetInnerMainContainer(ECS::CComponentManager& _Component_Manager)
	{
		return static_cast<Core::TCVectorContainer<T>*>(_Component_Manager.tMain_Container_[_Component_Manager.CreateComponentContainer<T>()]);
	}

	template <typename T>
	Core::TCVectorContainer<unsigned int>* GetInnerIndexContainer(ECS::CComponentManager& _Component_Manager)
	{
		return static_cast<Core::TCVectorContainer<unsigned int>*>(_Component_Manager.tOrdered_Container_[_Component_Manager.CreateComponentContainer<T>()]);
}

}

#endif
