#ifndef COLLIDER_COMPONENT
#define COLLIDER_COMPONENT

#include "Vector.hpp"

namespace GLVM::ecs::components
{
	class collider
	{
    public:
        bool bGround_Collision_ = false;
		bool roofCollision = false;
        bool bWall_Collision_ = false;
	};
}

#endif
