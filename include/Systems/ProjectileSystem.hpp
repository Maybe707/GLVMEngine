#ifndef PROJECTILE_SYSTEM
#define PROJECTILE_SYSTEM

#include "ISystem.hpp"
#include "Components/ProjectileComponent.hpp"
#include "Vector.hpp"
#include "ComponentManager.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "chelik.hpp"
#include "Globals.hpp"
#include "TextureManager.hpp"

namespace GLVM::ecs
{
    class CProjectileSystem : public ISystem
    {
    public:
        float fYaw = -90.0f;
        float fPitch = 0.0f;
        float fLast_X = 1920.0f / 2.0f;
        float fLast_Y = 1080.0f / 2.0f;
        bool bFirst_Mouse = true;
        core::CStack&              Input_Stack_;
        float                      fProjectile_Accumulator_ = 2.0f; 
		float                      _dOffset;

        CProjectileSystem(core::CStack& _input_Stack);
        void Update() override;
        void CalculateProjectile(ecs::CComponentManager* pComponent_Manager,
                                 unsigned int iEntity_refMove,
                                 components::beholder& view_Component);
        Vector<float, 3> GetDirectionVector(components::beholder& _view_Component);
    };
}

#endif
