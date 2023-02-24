#ifndef COMPONENT_MANAGER
#define COMPONENT_MANAGER

#include "Vector.hpp"
#include <cassert>
#include <iostream>
#include "IContainer.hpp"
//#include "Components/VertexComponent.hpp"
#include <mutex>
#include <assert.h>

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
				worldComponentsContainer.Push(componentContainer);
            
				core::vector<Entity>* sparseEntitiesMapToComponents =
					new core::vector<Entity>;    ///< Create ID's component container.
				worldSparseEntitiesMapToComponents.Push(sparseEntitiesMapToComponents);

				core::vector<Entity>* denseEntitiesMapToComponents =
					new core::vector<Entity>;    ///< Create ID's component container.
				worldDenseComponentsMapToEntities.Push(denseEntitiesMapToComponents);
				
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
//			std::cout << typeid(componentType).name() << std::endl;
			if ( checkAvailability( sparse, dense, entity ) ) {
				return;
			}
			
			if ( entity >= sparse.GetSize() ) {
				sparse.Resize(entity + 1);
			}

			assert( dense.GetSize() == components.GetSize() );
			
			sparse[entity] = dense.GetSize();
			dense.Push(entity);
			components.Push(Component);
		}

		bool checkAvailability( core::vector<Entity>& sparse,
								core::vector<Entity>& dense,
								Entity entity );
		
        /// Allow to give a various components to chosen entity.
        
        template <typename componentType1, typename componentType2, typename... Args>
		void CreateComponent(Entity& entity) {
            CreateComponent<componentType2, Args...>(entity);
            CreateComponent<componentType1>(entity);
        }

        template <typename componentType, typename... Args>
		core::vector<Entity> collectLinkedEntities() {
			core::vector<Entity>& dense = *static_cast<core::vector<Entity>*>
				( worldDenseComponentsMapToEntities[CreateComponentContainer<componentType>()] );

			core::vector<Entity> returnVector;
			for ( unsigned int i = 0; i < dense.GetSize(); ++i ) {
				if ( multiCheckAvailability<Args...>(dense[i]) )
					returnVector.Push(dense[i]);
			}

			return returnVector;
		}

		template <typename... Args>
		bool multiCheckAvailability(Entity entity) {
			return (multiCheckAvailabilityBase<Args>(entity) && ...);
		}

		template <typename componentType>
		bool multiCheckAvailabilityBase(Entity entity) {
			core::vector<Entity>& sparse = *static_cast<core::vector<Entity>*>
				(worldSparseEntitiesMapToComponents[CreateComponentContainer<componentType>()]);
			core::vector<Entity>& dense = *static_cast<core::vector<Entity>*>
				(worldDenseComponentsMapToEntities[CreateComponentContainer<componentType>()]);

			return checkAvailability(sparse, dense, entity);
		}
		
        template <typename componentType>
        componentType* GetComponent(const Entity& entity)
        {
            unsigned int localContainerID;
            localContainerID = CreateComponentContainer<componentType>();

			core::vector<Entity>& sparse = *static_cast<core::vector<Entity>*>
				(worldSparseEntitiesMapToComponents[localContainerID]);
			core::vector<Entity>& dense = *static_cast<core::vector<Entity>*>
				(worldDenseComponentsMapToEntities[localContainerID]);
			core::vector<componentType>& components =
				*static_cast<core::vector<componentType>*>(worldComponentsContainer[localContainerID]);

			if ( checkAvailability( sparse, dense, entity ) ) {
				Entity componentIndex = sparse[entity];
				return &components[componentIndex];
			} else {
				componentType* nullPtrComponent = nullptr;
				return nullPtrComponent;
			}
        }
        
        /**************************************************************************************
         * Dont need to delete real component in this method. Because systems dont work with
         * component without indices for that component in ordered container.
         **************************************************************************************/
        
		template <typename componentType>
		void RemoveComponent(Entity& entity)
			{
				unsigned int localContainerID;
				localContainerID = CreateComponentContainer<componentType>();

				core::vector<Entity>& sparse = *static_cast<core::vector<Entity>*>
					(worldSparseEntitiesMapToComponents[localContainerID]);
				core::vector<Entity>& dense = *static_cast<core::vector<Entity>*>
					(worldDenseComponentsMapToEntities[localContainerID]);
				core::vector<componentType>& components = *static_cast<core::vector<componentType>*>
					(worldComponentsContainer[localContainerID]);

				if ( checkAvailability( sparse, dense, entity ) ) {
					assert( dense.GetSize() == components.GetSize() );
					
					Entity indexInDenseOfRemovableEntity = sparse[entity];
					Entity indexInSparseOfSwapableEntity = dense.GetHead();
					const componentType& componentFromLastIndex = components.GetHead();
					dense[indexInDenseOfRemovableEntity] = indexInSparseOfSwapableEntity;
					dense.Pop();
					components[indexInDenseOfRemovableEntity] = componentFromLastIndex;
					components.Pop();
					sparse[indexInSparseOfSwapableEntity] = indexInDenseOfRemovableEntity;
				}
			}
		
		unsigned int GetContainerID();

		template <typename componentType>
		core::VectorIterator<componentType> GetComponentContainerTest() {
			core::vector<componentType>* componentVector = static_cast<core::vector<componentType>*>(worldComponentsContainer[CreateComponentContainer<componentType>()]);
			core::VectorIterator<componentType> iterator(*componentVector);
			return iterator;
		}

		// template <typename componentType>
		// core::VectorIterator<Entity> GetEntityContainer() {
		// 	core::vector<Entity>* entityVector = static_cast<core::vector<Entity>*>(worldDenseComponentsMapToEntities[CreateComponentContainer<componentType>()]);
		// 	core::VectorIterator<Entity> iterator(*entityVector);
		// 	return iterator;
		// }

		template <typename componentType>
		core::vector<componentType>* GetComponentContainer()
			{
				return static_cast<core::vector<componentType>*>(worldComponentsContainer[CreateComponentContainer<componentType>()]);
			}

		template <typename componentType>
		core::vector<Entity>* GetEntityContainer()
			{
				return static_cast<core::vector<Entity>*>(worldDenseComponentsMapToEntities[CreateComponentContainer<componentType>()]);
			}
	};


}

#endif
