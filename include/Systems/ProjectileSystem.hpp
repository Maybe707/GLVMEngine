#ifndef PROJECTILE_SYSTEM
#define PROJECTILE_SYSTEM

#include "ISystem.hpp"
#include "Components/ProjectileComponent.hpp"
#include "VectorContainer.hpp"
#include "ComponentManager.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Globals.hpp"

namespace GLVM::ECS
{
    class CProjectileSystem : public ISystem
    {
    public:
        float fYaw = -90.0f;
        float fPitch = 0.0f;
        float fLast_X = 1920.0f / 2.0f;
        float fLast_Y = 1080.0f / 2.0f;
        bool bFirst_Mouse = true;
        
        void Update() override;
        Vector<float, 3> GetDirectionVector(ECS::STransformComponent& _Player, ECS::CViewComponent& _view_Component);
    };
}

#endif
