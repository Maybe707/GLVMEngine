#ifndef COMPONENT_MANAGER
#define COMPONENT_MANAGER

#include "Components/AnimationMoveComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Vector.hpp"
#include <cassert>
#include <iostream>
#include "IContainer.hpp"
//#include "Components/VertexComponent.hpp"
#include <mutex>

typedef unsigned int Entity;

namespace GLVM::ecs
{
	class CComponentManager
	{
        static CComponentManager* pInstance_;
        static std::mutex  Mutex_;
		
        CComponentManager();
        ~CComponentManager();

		template <typename componentType>
		unsigned int CreateComponentContainer()
			{
				static unsigned int localContainerID = 0;
				static bool existComponentContainerFlag = false;
				if(existComponentContainerFlag)  
					return localContainerID;
                                                                                                                
				localContainerID = componentsContainerID;    ///< Give a value of global component container ID's counter to local container ID of current component type.
				existComponentContainerFlag = true;
            
				core::vector<componentType>* componentContainer =
					new core::vector<componentType>;    ///< Create component container of current type.
				worldComponentsContainer.Insert(componentContainer, componentsContainerID);
            
				core::vector<Entity>* sparseEntitiesMapToComponents =
					new core::vector<Entity>;    ///< Create ID's component container.
				worldSparseEntitiesMapToComponents.Insert(sparseEntitiesMapToComponents, componentsContainerID);

				core::vector<Entity>* denseEntitiesMapToComponents =
					new core::vector<Entity>;    ///< Create ID's component container.
				worldDenseComponentsMapToEntities.Insert(denseEntitiesMapToComponents, componentsContainerID);
				
//				std::cout << typeid(Component_Type).name() << std::endl;			
				++componentsContainerID;
				return localContainerID;
			}
        
	public:
		inline static unsigned int componentsContainerID = 0;
		core::vector<core::IContainer*> worldComponentsContainer;    ///< Contains all local containers for diferent types of components.
		core::vector<core::vector<Entity>*> worldSparseEntitiesMapToComponents;    ///< Contains all local container with IDs for diferent types of components.
		core::vector<core::vector<Entity>*> worldDenseComponentsMapToEntities;

        CComponentManager(CComponentManager& componentManager) = delete;         ///< Dont need to make cope because of singleton property.
        void operator=(const CComponentManager& componentManager) = delete;      ///< Dont need assignment operator because of singleton property.
       static CComponentManager* GetInstance();                          ///< It possibly to get only one instance of this class whith this method.
                
		template <typename componentType>
		void CreateComponent(const Entity& entity)
		{
			unsigned int localContainerID = 0; ///< Index for world components and world ID's containers.
			componentType Component;
			localContainerID = CreateComponentContainer<componentType>();

			core::vector<Entity>& sparse = *static_cast<core::vector<Entity>*>
				(worldSparseEntitiesMapToComponents[localContainerID]);
			core::vector<Entity>& dense = *static_cast<core::vector<Entity>*>
				(worldDenseComponentsMapToEntities[localContainerID]);
			core::vector<componentType>& components = *static_cast<core::vector<componentType>*>
				(worldComponentsContainer[localContainerID]);
			
			for (int i = 0; i < sparse.GetSize(); ++i) {
				if (sparse[i] == entity)
					return;
			}

			// unsigned int maxValue = 0;
			// maxValue = ~maxValue;
			
			unsigned int topComponentIndex = dense.GetSize();
			sparse.Insert(topComponentIndex, entity);
			dense.Push(entity);
			components.Push(Component);
				
			// static_cast<core::vector<componentType>*>(worldComponentsContainer[localContainerID])->Insert(Component, entity);
			// entitiesContainer->Push(entity);
//			return (*static_cast<core::vector<Component_Type>*>(tWorld_Components_Container_[u_iLocal_Container_ID]))[_u_iEntity];
		}

        /// Allow to give a various components to chosen entity.
        
        template <typename componentType, typename componentType2, typename... Args>
		void CreateComponent(Entity& entity)
		{
            CreateComponent<componentType2, Args...>(entity);
            CreateComponent<componentType>(entity);
        }

        template <typename componentType>
        componentType& GetComponent(const Entity& entity)
        {
            unsigned int localContainerID;
            localContainerID = CreateComponentContainer<componentType>();
			
			core::vector<Entity>& components =
				*static_cast<core::vector<Entity>*>(worldSparseEntitiesMapToComponents[localContainerID]);
			core::VectorIterator<Entity> iterator = components.Find(entity);
			
			if ( localContainerID || !iterator.ValidStatus() )
				return iterator.Current();
			
            return components[entity];
        }
        
        /**************************************************************************************
         * Dont need to delete real component in this method. Because systems dont work with
         * component without indices for that component in ordered container.
         **************************************************************************************/
        
		template <typename componentType>
		void RemoveComponent(Entity& entity)
			{
				// core::vector<Entity>* entityVector = static_cast<core::vector<Entity>*>
				// 	(worldSparseEntitiesMapToComponents[CreateComponentContainer<componentType>()]);
				// core::vector<componentType>* componentVector = static_cast<core::vector<componentType>*>
				// 	(worldComponentsContainer[CreateComponentContainer<componentType>()]);

				unsigned int localContainerID;
				localContainerID = CreateComponentContainer<componentType>();

				if (localContainerID)
					return;
				
				core::vector<Entity>& sparse = *static_cast<core::vector<Entity>*>
					(worldSparseEntitiesMapToComponents[localContainerID]);
				core::vector<Entity>& dense = *static_cast<core::vector<Entity>*>
					(worldDenseComponentsMapToEntities[localContainerID]);
				core::vector<componentType>& components = *static_cast<core::vector<componentType>*>
					(worldComponentsContainer[localContainerID]);

				unsigned int entityIndexInDenseArray = sparse[entity];
				dense.Swap(entityIndexInDenseArray, dense.GetHead());
				unsigned int entityIndexInSparseArray = dense.GetHead();
				dense.Pop();
				dense[entityIndexInSparseArray] = entityIndexInDenseArray;
				components.Remove(dense.GetHead());
//				componentVector->Remove(entity);
				
//				core::VectorIterator<unsigned int> iterator = entityVector->Find(entity);
 				// if ( !iterator.ValidStatus() ) {
				// 	entityVector->Swap(iterator.Current(), entityVector->GetHead());
				// 	entityVector->Pop();
				// }
			}
		
		unsigned int GetContainerID();

		template <typename componentType>
		core::vector<componentType>* GetComponentContainer()
			{
				return static_cast<core::vector<componentType>*>(worldComponentsContainer[CreateComponentContainer<componentType>()]);
			}

		template <typename componentType>
		core::vector<Entity>* GetEntityContainer()
			{
				return static_cast<core::vector<Entity>*>(worldSparseEntitiesMapToComponents[CreateComponentContainer<componentType>()]);
			}
	};


}

#endif
