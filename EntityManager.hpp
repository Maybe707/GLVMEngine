#ifndef ENTITY_MANAGER
#define ENTITY_MANAGER

#include "ConstVectorContainer.hpp"
#include "VectorContainer.hpp"

using Entity_Type_ID = unsigned int;
using Entity_Object_ID = unsigned int;

namespace GLVM::ECS
{
	class CEntityManager
	{
		inline static Entity_Type_ID u_iType_ID_ = 0;		
 		Core::TCVectorContainer<Entity_Object_ID> tRemoved_Objects_Registry_;
		Core::TCConstVectorContainer<Entity_Object_ID> tActive_Objects_Registry_;
	public:
		void CreateEntity(Entity_Object_ID& _Entity_Object_ID)
		{
			if(tRemoved_Objects_Registry_.GetSize() > k_iNull)
			{
				_Entity_Object_ID = tRemoved_Objects_Registry_.GetFirstItem();
				tActive_Objects_Registry_.Push(tRemoved_Objects_Registry_.GetFirstItem());
				tRemoved_Objects_Registry_.RemoveFirstItem();
			}
			else
			{
				tActive_Objects_Registry_.Push(u_iType_ID_);
				_Entity_Object_ID = u_iType_ID_;
				++u_iType_ID_;
			}
		}
		
		void RemoveEntity(Entity_Object_ID& _Entity_Object_ID)
		{
			tRemoved_Objects_Registry_.Push(_Entity_Object_ID);
			tActive_Objects_Registry_[_Entity_Object_ID] = k_iUint_Max;
		}
	};
}

#endif
