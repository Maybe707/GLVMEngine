#ifndef PROJECTILE_SYSTEM
#define PROJECTILE_SYSTEM

#include "ISystem.hpp"
#include "Components/ProjectileComponent.hpp"
#include "VectorContainer.hpp"

namespace GLVM::ECS
{
    class CProjectileSystem : public ISystem
    {
    public:
        void Update(ECS::CComponentManager& _Component_Manager, Core::CEvent& _Event) override;
    };
}

#endif
