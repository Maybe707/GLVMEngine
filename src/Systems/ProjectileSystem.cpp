#include "ComponentManager.hpp"
#include "Components/ProjectileComponent.hpp"
#include <Systems/ProjectileSystem.hpp>

namespace GLVM::ECS
{
    void CProjectileSystem::Update()
    {
        CComponentManager* pComponent_Manager = GLVM::ECS::CComponentManager::GetInstance();
        Core::TCVectorContainer<unsigned int>* pEntity_Container_refProjectile =
            GetInnerIDsContainer<CProjectileComponent>(*pComponent_Manager);
        unsigned int uiVector_Projectile_Size = pEntity_Container_refProjectile->GetSize();
        
        for(int i = 0; i < uiVector_Projectile_Size; ++i)
        {
            unsigned int uiEntity_refProjectile = (*pEntity_Container_refProjectile)[i];

            
        }
    }
}
