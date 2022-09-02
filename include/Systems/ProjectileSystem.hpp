#ifndef PROJECTILE_SYSTEM
#define PROJECTILE_SYSTEM

#include "ISystem.hpp"
#include "Components/ProjectileComponent.hpp"
#include "VectorContainer.hpp"
#include "ComponentManager.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "chelik.hpp"
#include "Globals.hpp"
#include "Systems/TextureSystem.hpp"

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
        Core::CStack&              Input_Stack_;
        float                      fProjectile_Accumulator_ = 2.0f; 
		float                      _dOffset;

        CProjectileSystem(Core::CStack& _input_Stack);
        void Update() override;
        void CalculateProjectile(ECS::CComponentManager* pComponent_Manager,
                                 unsigned int iEntity_refMove,
                                 CViewComponent& view_Component);
        Vector<float, 3> GetDirectionVector(ECS::CViewComponent& _view_Component);
    };
}

#endif
