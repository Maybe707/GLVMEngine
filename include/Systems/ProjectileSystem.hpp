#ifndef PROJECTILE_SYSTEM
#define PROJECTILE_SYSTEM

#include "ISystem.hpp"
#include "Components/ProjectileComponent.hpp"
#include "VectorContainer.hpp"
#include "ComponentManager.hpp"

namespace GLVM::ECS
{
    class CProjectileSystem : public ISystem
    {
    public:
        void Update() override;
    };
}

#endif
