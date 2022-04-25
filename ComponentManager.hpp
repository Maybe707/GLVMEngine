#ifndef COMPONENT_MANAGER
#define COMPONENT_MANAGER

#include "AnimationMoveComponent.hpp"
#include "ColliderComponent.hpp"
#include "MoveComponent.hpp"
#include "VectorContainer.hpp"
#include <iostream>
#include "IContainer.hpp"
#include "VertexComponent.hpp"

typedef unsigned int Entity_ID;

namespace GLVM::ECS
{
	class CComponentManager
	{
	public:
		inline static unsigned int s_iComponents_Container_ID = 0;
		Core::TCVectorContainer<Core::IContainer*> tWorld_Components_Container_;    ///< Contains all local containers for diferent types of components.
		Core::TCVectorContainer<Core::TCVectorContainer<Entity_ID>*> tWorld_IDs_Container;    ///< Contains all local container with IDs for diferent types of components.
        
		template <typename Component_Type>
		unsigned int CreateComponentContainer()
		{
			static unsigned int s_iLocal_Container_ID = 0;
			static bool s_bExist_Component_Container_Flag = false;
			if(s_bExist_Component_Container_Flag)  
				return s_iLocal_Container_ID;
                                                                                                                
			s_iLocal_Container_ID = s_iComponents_Container_ID;    ///< Give a value of global component container ID's counter to local container ID of current component type.
			s_bExist_Component_Container_Flag = true;
            
			Core::TCVectorContainer<Component_Type>* pComponent_Container =
				new Core::TCVectorContainer<Component_Type>;    ///< Create component container of current type.
			tWorld_Components_Container_.Insert(pComponent_Container, s_iComponents_Container_ID);
            
			Core::TCVectorContainer<Entity_ID>* pRow_Ordered_IDs_Container =
				new Core::TCVectorContainer<Entity_ID>;    ///< Create ID's component container.
			tWorld_IDs_Container.Insert(pRow_Ordered_IDs_Container, s_iComponents_Container_ID);
            
			++s_iComponents_Container_ID;
			return s_iLocal_Container_ID;
		}
        
		template <typename Component_Type>
		void CreateComponent(const Entity_ID& _u_iEntity)
		{
			unsigned int u_iLocal_Container_ID = 0; ///< Index for world components and world ID's containers.
			Component_Type Component;
			u_iLocal_Container_ID = CreateComponentContainer<Component_Type>();

			static_cast<Core::TCVectorContainer<Component_Type>*>(tWorld_Components_Container_[u_iLocal_Container_ID])->Insert(Component, _u_iEntity);
			static_cast<Core::TCVectorContainer<Entity_ID>*>(tWorld_IDs_Container[u_iLocal_Container_ID])->Push(_u_iEntity);
//			return (*static_cast<Core::TCVectorContainer<Component_Type>*>(tWorld_Components_Container_[u_iLocal_Container_ID]))[_u_iEntity];
		}

        /// Allow to give a various components to chosen entity.
        
        template <typename Component_Type, typename Component_Type2, typename... Args>
		void CreateComponent(Entity_ID& _u_iEntity)
		{
            CreateComponent<Component_Type2, Args...>(_u_iEntity);
            CreateComponent<Component_Type>(_u_iEntity);
        }

        template <typename Component_Type>
        Component_Type& GetComponent(const Entity_ID& _u_iEntity)
        {
            unsigned int u_iLocal_Container_ID;
            u_iLocal_Container_ID = CreateComponentContainer<Component_Type>();

            return (*static_cast<Core::TCVectorContainer<Component_Type>*>(tWorld_Components_Container_[u_iLocal_Container_ID]))[_u_iEntity];
        }
        
        /**************************************************************************************
         * Dont need to delete real component in this method. Because systems dont work with
         * component without indices for that component in ordered container.
         **************************************************************************************/
        
		template <typename Component_Type>
		void RemoveComponent(Entity_ID& _u_iEntity)
		{
//			static_cast<Core::TCConstVectorContainer<S>*>(tWorld_Components_Container_[CreateComponentContainer<S>()])->Remove(_u_iEntity);
			static_cast<Core::TCVectorContainer<Entity_ID>*>(tWorld_IDs_Container[CreateComponentContainer<Component_Type>()])->RemoveItem(_u_iEntity);
		}
		
		unsigned int GetContainerID()
		{
			return s_iComponents_Container_ID;
		}
		
		~CComponentManager()
		{
			for(int i = 0, iSize_Main = tWorld_Components_Container_.GetSize(); i < iSize_Main; ++i)
			{
				delete tWorld_Components_Container_[i];
				tWorld_Components_Container_[i] = nullptr;
			}
			for(int j = 0, iSize_Ordered = tWorld_IDs_Container.GetSize(); j < iSize_Ordered; ++j)
			{
				delete tWorld_IDs_Container[j];
				tWorld_IDs_Container[j] = nullptr;
			}
		}
	};

	template <typename Component_Type>
	Core::TCVectorContainer<Component_Type>* GetInnerComponentContainer(ECS::CComponentManager& _Component_Manager)
	{
		return static_cast<Core::TCVectorContainer<Component_Type>*>(_Component_Manager.tWorld_Components_Container_[_Component_Manager.CreateComponentContainer<Component_Type>()]);
	}

	template <typename Component_Type>
	Core::TCVectorContainer<Entity_ID>* GetInnerIDsContainer(ECS::CComponentManager& _Component_Manager)
	{
		return static_cast<Core::TCVectorContainer<Entity_ID>*>(_Component_Manager.tWorld_IDs_Container[_Component_Manager.CreateComponentContainer<Component_Type>()]);
}

}

#endif
