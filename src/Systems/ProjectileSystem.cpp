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

        Core::TCVectorContainer<unsigned int>* pEntity_Container_refMove =
            ECS::GetInnerIDsContainer<ECS::SMoveComponent>(*pComponent_Manager);
        Core::TCVectorContainer<unsigned int>* pEntity_Container_refView =
            ECS::GetInnerIDsContainer<ECS::CViewComponent>(*pComponent_Manager);
        unsigned int iEntity_refView = (*pEntity_Container_refView)[0];
        ECS::CViewComponent& view_Component = pComponent_Manager->GetComponent<ECS::CViewComponent>(iEntity_refView);
        
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
            // Vector<float, 3> temp_vec(0.0f);
            // temp_vec = GetDirectionVector(pComponent_Manager->GetComponent<STransformComponent>(iEntity_refView), view_Component);
//            pComponent_Manager->GetComponent<STransformComponent>(uiEntity_refProjectile).tVertex += temp_vec * 0.1;

            ECS::CProjectileComponent& rProjectileComponent = pComponent_Manager->GetComponent<ECS::CProjectileComponent>(uiEntity_refProjectile);
            pComponent_Manager->GetComponent<STransformComponent>(uiEntity_refProjectile).tVertex += rProjectileComponent.forward_ * 0.1f;
            
            // pComponent_Manager->GetComponent<STransformComponent>(uiEntity_refProjectile).tVertex[0] +=
            //     pComponent_Manager->GetComponent<STransformComponent>(uiEntity_refProjectile).tVertex[0] / 10;
            // pComponent_Manager->GetComponent<STransformComponent>(uiEntity_refProjectile).tVertex[1] +=
            //     pComponent_Manager->GetComponent<STransformComponent>(uiEntity_refProjectile).tVertex[1] / 10;
            // pComponent_Manager->GetComponent<STransformComponent>(uiEntity_refProjectile).tVertex[2] +=
            //     pComponent_Manager->GetComponent<STransformComponent>(uiEntity_refProjectile).tVertex[2] / 10;
        }
    }

    Vector<float, 3> CProjectileSystem::GetDirectionVector(STransformComponent& _Player, CViewComponent& _view_Component)
    {
                Matrix<float, 4> tView_Matrix(1.0f);
        const float kSensitivity = 0.1f;

        fYaw = g_eEvent.mouse_Pointer_Position_.iOffset_X;
        fPitch = g_eEvent.mouse_Pointer_Position_.iOffset_Y;
        fYaw *= kSensitivity;
        fPitch *= kSensitivity;

        g_eEvent.mouse_Pointer_Position_.fPitch_ = fPitch;
        g_eEvent.mouse_Pointer_Position_.fYaw_ = fYaw;
        
        if(fPitch > 89.0f)
            fPitch = 89.0f;
        if(fPitch < -89.0f)
            fPitch = -89.0f;

        Vector<float, 3> front;
        front[0] = std::cos(Radians(fYaw)) * std::cos(Radians(fPitch));
        front[1] = std::sin(Radians(fPitch));
        front[2] = std::sin(Radians(fYaw)) * std::cos(Radians(fPitch));
        _view_Component.Front_Camera = Normalize(front);

        return _view_Component.Front_Camera;
    }    
}
