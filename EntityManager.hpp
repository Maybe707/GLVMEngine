#ifndef ENTITY_MANAGER
#define ENTITY_MANAGER

#include "VectorContainer.hpp"
#include "ComponentManager.hpp"

using Entity_Type_ID = unsigned int;
using Entity_Object_ID = unsigned int;

namespace GLVM::ECS
{
	class CEntityManager
	{
    public:                                                                   ///< !!!!!DELETE!!!!!!!!!!!!!!!11
		inline static Entity_Type_ID u_iType_ID_ = 1;		
 		Core::TCVectorContainer<Entity_Object_ID> tRemoved_Entity_Registry_;
		Core::TCVectorContainer<Entity_Object_ID> tActive_Entity_Registry_;
	public:
		void CreateEntity(Entity_Object_ID& _Entity_Object_ID)
		{
			if(tRemoved_Entity_Registry_.GetSize() > k_iNull)
			{
				_Entity_Object_ID = tRemoved_Entity_Registry_.GetFirstItem();
				tActive_Entity_Registry_.Push(tRemoved_Entity_Registry_.GetFirstItem());
				tRemoved_Entity_Registry_.RemoveFirstItem();
			}
			else
			{
				tActive_Entity_Registry_.Push(u_iType_ID_);
				_Entity_Object_ID = u_iType_ID_;
				++u_iType_ID_;
			}
		}

        ///< Dont need to delete real component in this method. Because systems dont work with component without indices for
        ///< that component in ordered container.
        
		void RemoveEntity(Entity_Object_ID& _Entity_Object_ID, CComponentManager& _ComponentManager)
		{
			tRemoved_Entity_Registry_.Push(_Entity_Object_ID);
			tActive_Entity_Registry_[_Entity_Object_ID - 1] = k_iUint_Max;
			for(int i = 0, iSize = _ComponentManager.tOrdered_Container_.GetSize();
				i < iSize; ++i)
			{
				static_cast<Core::TCVectorContainer<unsigned int>*>(_ComponentManager.tOrdered_Container_[i])->RemoveItem(_Entity_Object_ID);
			}
		}
	};
}

#endif
