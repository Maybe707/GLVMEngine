#include "ComponentManager.hpp"
#include "Components/ProjectileComponent.hpp"
#include <Systems/ProjectileSystem.hpp>

namespace GLVM::ECS
{
    void CProjectileSystem::Update(ECS::CComponentManager& _Component_Manager, Core::CEvent& _Event)
    {
        Core::TCVectorContainer<unsigned int>* pEntity_Container_refProjectile =
            GetInnerIDsContainer<CProjectileComponent>(_Component_Manager);
        unsigned int uiVector_Projectile_Size = pEntity_Container_refProjectile->GetSize();
        
        for(int i = 0; i < uiVector_Projectile_Size; ++i)
        {
            unsigned int uiEntity_refProjectile = (*pEntity_Container_refProjectile)[i];

            
        }
    }
}
