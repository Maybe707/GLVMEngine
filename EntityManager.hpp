#ifndef ENTITY_MANAGER
#define ENTITY_MANAGER

#include "VectorContainer.hpp"
#include "ComponentManager.hpp"

typedef unsigned int Entity_ID;  

namespace GLVM::ECS
{
	class CEntityManager
	{
    public:                                                                   ///< !!!!!DELETE!!!!!!!!!!!!!!!11
		inline static Entity_ID u_iID = 0;		
 		Core::TCVectorContainer<Entity_ID> tRemoved_Entity_Registry_;
		Core::TCVectorContainer<Entity_ID> tActive_Entity_Registry_;
	public:
		void CreateEntity(Entity_ID& _Entity_ID)
		{
			if(tRemoved_Entity_Registry_.GetSize() > k_iNull)    ///< Check out wether or not free ID in removed entities registry.
			{
				_Entity_ID = tRemoved_Entity_Registry_.GetFirstItem();
				tActive_Entity_Registry_.Push(tRemoved_Entity_Registry_.GetFirstItem());
				tRemoved_Entity_Registry_.RemoveFirstItem();
			}
			else
			{
				tActive_Entity_Registry_.Push(u_iID);
				_Entity_ID = u_iID;
				++u_iID;
			}
		}

        /**************************************************************************************
         * Dont need to delete real component in this method. Because systems dont work with
         * component without indices forthat component in ordered container.
         **************************************************************************************/
        
		void RemoveEntity(Entity_ID& _Entity_ID, CComponentManager& _ComponentManager)
		{
			tRemoved_Entity_Registry_.Push(_Entity_ID);
			tActive_Entity_Registry_[_Entity_ID] = k_iUint_Max;  
			for(int i = 0, iSize = _ComponentManager.tWorld_IDs_Container.GetSize();
				i < iSize; ++i)
			{
				static_cast<Core::TCVectorContainer<unsigned int>*>(_ComponentManager.tWorld_IDs_Container[i])->RemoveItem(_Entity_ID);
			}
		}
	};
}

#endif
