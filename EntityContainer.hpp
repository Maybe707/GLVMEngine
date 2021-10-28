#ifndef ENTITY_CONTAINER
#define ENTITY_CONTAINER

#include "ConstVectorContainer.hpp"

using Entity_Type_ID = unsigned int;
using Entity_Object_ID = unsigned int;

namespace GLVM::ECS
{
	class CEntityContainer
	{
		Core::TCConstVectorContainer<Entity_Object_ID> tRemoved_Objects_Registry_;
		Core::TCConstVectorContainer<Entity_Object_ID> tActive_Objects_Registry_;
		inline static Entity_Type_ID u_iType_ID_ = 0;		
	public:
		void CreateEntity(Entity_Object_ID& _Entity_Object_ID)
		{
			;
		}
	};
}

#endif
