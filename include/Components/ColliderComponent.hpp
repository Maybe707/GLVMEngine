#ifndef COLLIDER_COMPONENT
#define COLLIDER_COMPONENT

#include "VectorContainer.hpp"

namespace GLVM::ECS
{
    struct Collider
    {
        bool bGround_Collision_ = false;
        bool bWall_Collision_ = false;

        unsigned int uiGround_Collider_ = 0;
        unsigned int uiWall_Collider_ = 0;
    };
    
	class CColliderComponent
	{
    public:
//        Core::TCVectorContainer<Collider> aColliders_Container_;
        bool bGround_Collision_ = false;
        bool bWall_Collision_ = false;

        unsigned int uiGround_Collider_ = 0;
        unsigned int uiWall_Collider_ = 0;
	};
}

#endif
