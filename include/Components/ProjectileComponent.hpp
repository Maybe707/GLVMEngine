#ifndef PROJECTILE_COMPONENT
#define PROJECTILE_COMPONENT

namespace GLVM::ecs::components
{
    class projectile
    {
    public:
		unsigned int owner;
        bool bCollision_Status_ = false;
        float fDamage_;
        float fSpeed_;
        float fFlying_Range_;
    };
}

#endif
