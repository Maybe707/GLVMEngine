#include "ComponentManager.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ProjectileComponent.hpp"
#include "Components/TextureComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "EntityManager.hpp"
#include <Systems/ProjectileSystem.hpp>

namespace GLVM::ECS
{
    void CProjectileSystem::Update()
    {
        CComponentManager* pComponent_Manager = GLVM::ECS::CComponentManager::GetInstance();
        CEntityManager* pEntity_Manager       = GLVM::ECS::CEntityManager::GetInstance();
        Core::TCVectorContainer<unsigned int>* pEntity_Container_refProjectile =
            GetInnerIDsContainer<CProjectileComponent>(*pComponent_Manager);
        unsigned int uiVector_Projectile_Size = pEntity_Container_refProjectile->GetSize();
        
        for(int i = 0; i < uiVector_Projectile_Size; ++i)
        {
            unsigned int uiEntity_refProjectile = (*pEntity_Container_refProjectile)[i];

//           std::cout << "Entities: " << uiEntity_refProjectile << std::endl;
            
            if(pComponent_Manager->GetComponent<CColliderComponent>(uiEntity_refProjectile).bWall_Collision_)
            {
                std::cout << "FLAG: " << pComponent_Manager->GetComponent<CColliderComponent>(uiEntity_refProjectile).bWall_Collision_ << std::endl;
                  pEntity_Manager->RemoveEntity(uiEntity_refProjectile, *pComponent_Manager);
//                pComponent_Manager->RemoveComponent<CTextureComponent>(uiEntity_refProjectile);
//                pComponent_Manager->GetComponent<STransformComponent>(uiEntity_refProjectile).tVertex -= 0.2;
//                std::cout << "We are finaly here: " << uiEntity_refProjectile << std::endl;
                continue;
            }
            pComponent_Manager->GetComponent<STransformComponent>(uiEntity_refProjectile).tVertex += 0.1;
        }
    }
}
