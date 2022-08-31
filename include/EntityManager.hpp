#ifndef ENTITY_MANAGER
#define ENTITY_MANAGER

#include "VectorContainer.hpp"
#include "ComponentManager.hpp"

typedef unsigned int Entity_ID;  

namespace GLVM::ECS
{
	class CEntityManager
	{
        static CEntityManager* pInstance_;
        static std::mutex  Mutex_;

        CEntityManager();
        ~CEntityManager();
        
    public:                                                                   ///< !!!!!DELETE!!!!!!!!!!!!!!!11
		inline static Entity_ID u_iID = 0;		
 		Core::TCVectorContainer<Entity_ID> tRemoved_Entity_Registry_;
		Core::TCVectorContainer<Entity_ID> tActive_Entity_Registry_;

        CEntityManager(CEntityManager& _entity_Manager) = delete;           ///< Dont need to make cope because of singleton property.
        void operator=(const CEntityManager& _entity_Manager) = delete;     ///< Dont need assignment operator because of singleton property.
        static CEntityManager* GetInstance();                      ///< It possibly to get only one instance of this class whith this method.
        
		void CreateEntity(Entity_ID& _Entity_ID);

        void RemoveEntity(Entity_ID& _Entity_ID, CComponentManager* _ComponentManager);
	};
}

#endif
