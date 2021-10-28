#ifndef COMPONENT_CONTAINER
#define COMPONENT_CONTAINER

#include "ConstVectorContainer.hpp"
#include "VectorContainer.hpp"
#include "IComponentContainer.hpp"

using Entity_Type_ID = unsigned int;
using Entity_Object_ID = unsigned int;

namespace GLVM::ECS
{
	template <class T>
	class TCComponentContainer : public IContainer
	{
		unsigned int u_iComponent_Container_ID;
		inline static Entity_Type_ID u_iType_ID_ = 0;		
 		Core::TCVectorContainer<T> tRemoved_Objects_Registry_;
		Core::TCConstVectorContainer<T> tActive_Objects_Registry_;
	public:
		void CreateComponent(Entity_Object_ID& _Entity_Object_ID)
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

		void RemoveComponent(Entity_Object_ID& _Entity_Object_ID)
		{
			tRemoved_Objects_Registry_.Push(_Entity_Object_ID);
			tActive_Objects_Registry_[_Entity_Object_ID] = k_iUint_Max;
		}
	};
}

#endif
