#ifndef COLLIDER_COMPONENT
#define COLLIDER_COMPONENT

#include "VectorContainer.hpp"

namespace GLVM::ecs
{
	class collider
	{
    public:
//        core::TCVectorContainer<Collider> aColliders_Container_;
        bool bGround_Collision_ = false;
        bool bWall_Collision_ = false;
        bool bPush_Collission = false;

        unsigned int uiGround_Collider_ = 0;
        unsigned int uiWall_Collider_ = 0;
	};
}

#endif
