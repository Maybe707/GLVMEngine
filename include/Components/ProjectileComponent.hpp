#ifndef PROJECTILE_COMPONENT
#define PROJECTILE_COMPONENT

#include "VertexMath.hpp"
#include "Components/VertexComponent.hpp"

namespace GLVM::ecs::components
{
    class projectile
    {
    public:
        bool bCollision_Status_ = false;
        float fDamage_;
        float fSpeed_;
        float fFlying_Range_;
    };
}

#endif
