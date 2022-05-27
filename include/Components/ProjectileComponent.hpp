#ifndef PROJECTILE_COMPONENT
#define PROJECTILE_COMPONENT

#include "VertexMath.hpp"

namespace GLVM::ECS
{
    class CProjectileComponent
    {
    public:
        bool bCollision_Status_ = false;
        float fDamage_;
        float fSpeed_;
        float fFlying_Range_;
    };
}

#endif
