#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include <Systems/ProjectileSystem.hpp>

namespace GLVM::ecs
{
    CProjectileSystem::CProjectileSystem(core::CStack& inputStack) : inputStack (inputStack)
    {}
    
    void CProjectileSystem::Update()
    {
		namespace cm = GLVM::ecs::components;
		
        ComponentManager* pComponent_Manager = GLVM::ecs::ComponentManager::GetInstance();
        EntityManager* pEntity_Manager       = GLVM::ecs::EntityManager::GetInstance();
    
        core::vector<unsigned int>* pEntity_Container_refMove =
			pComponent_Manager->GetEntityContainer<cm::move>();
        unsigned int u_iVector_Move_Size = pEntity_Container_refMove->GetSize();

        core::vector<unsigned int>* pEntity_Container_refView =
			pComponent_Manager->GetEntityContainer<cm::beholder>();
        unsigned int iEntity_refView = (*pEntity_Container_refView)[0];
        cm::beholder* view_Component = pComponent_Manager->GetComponent<cm::beholder>(iEntity_refView);
        
        float cameraSpeed = 5.5f * deltaFrameTime;            

        if(projectileCooldown > 0)
            projectileCooldown -= cameraSpeed;

        for(unsigned int i = 0; i < u_iVector_Move_Size; ++i) {
            for(int n = 0; n < 6; ++n) {
                unsigned int iEntity_refMove = (*pEntity_Container_refMove)[i];
                
                if(inputStack.SearchElement(core::EEvents::eMOUSE_LEFT_BUTTON) == core::EEvents::eMOUSE_LEFT_BUTTON) {
                    if(projectileCooldown <= 0) {
                        CalculateProjectile(pComponent_Manager,
                                            iEntity_refMove,
                                            *view_Component);
                        projectileCooldown = 2.0;
                    }
                }
            }
        }

        core::vector<unsigned int>* pEntity_Container_refProjectile =
			pComponent_Manager->GetEntityContainer<cm::projectile>();
        unsigned int uiVector_Projectile_Size = pEntity_Container_refProjectile->GetSize();

        for(unsigned int x = 0; x < uiVector_Projectile_Size; ++x) {
            unsigned int uiEntity_refProjectile = (*pEntity_Container_refProjectile)[x];
            cm::transform* rTransformProjectile = pComponent_Manager->GetComponent<cm::transform>(uiEntity_refProjectile);
			if ( rTransformProjectile != nullptr )
				rTransformProjectile->tPosition += rTransformProjectile->tForward * 0.2f;
        }

        GLVM::ecs::CTextureManager* TextureSystem = GLVM::ecs::CTextureManager::GetInstance();
		
        for(unsigned int i = 0; i < pEntity_Container_refProjectile->GetSize(); ++i) {

            unsigned int uiEntity_refProjectile = (*pEntity_Container_refProjectile)[i];
            if(pComponent_Manager->GetComponent<cm::collider>(uiEntity_refProjectile)->bWall_Collision_ ||
               pComponent_Manager->GetComponent<cm::collider>(uiEntity_refProjectile)->bGround_Collision_) {
                pEntity_Manager->RemoveEntity(uiEntity_refProjectile, pComponent_Manager);
                cm::material* textureProjectile = pComponent_Manager->GetComponent<cm::material>(uiEntity_refProjectile);
				if ( textureProjectile != nullptr )
					TextureSystem->UnbindTexture(*textureProjectile, uiEntity_refProjectile);     
            }
			std::cout << "Size: " << pComponent_Manager->GetEntityContainer<cm::projectile>()->GetSize() << std::endl;
			pComponent_Manager->GetEntityContainer<cm::projectile>()->Print();
			// std::cout << "Colliders container size: " << pComponent_Manager->GetEntityContainer<cm::collider>()->GetSize() << std::endl;
			// std::cout << "Projectiles container size 1: " << pEntity_Container_refProjectile->GetSize() << std::endl;
			// std::cout << "Projectiles container size 2: " << uiVector_Projectile_Size << std::endl;
			// std::cout << "entity: " << uiEntity_refProjectile << std::endl;
        }
    }

    void CProjectileSystem::CalculateProjectile(ecs::ComponentManager* componentManager,
                                              unsigned int entityRefMove,
												components::beholder& beholder) {
		namespace cm = GLVM::ecs::components;
        GLVM::ecs::CTextureManager* TextureSystem = GLVM::ecs::CTextureManager::GetInstance();
        
        unsigned int uiEntity_Projectile = ecs::EntityManager::GetInstance()->CreateEntity();
        ecs::ComponentManager::GetInstance()->CreateComponent<cm::vertex, cm::collider,
                                                               cm::transform, cm::material,
                                                               cm::projectile>(uiEntity_Projectile);

        // core::Sound::CSoundSample* pSound_Sample = new core::Sound::CSoundSample();
        // pSound_Sample->kPath_to_File_ = "../laser2.wav";
        // pSound_Sample->uiDuration_ = 5;
        // pSound_Sample->uiRate_ = 22050;
        // Sound_Engine_->GetSoundContainer().Push(pSound_Sample);
		componentManager->GetComponent<cm::vertex>(uiEntity_Projectile)->vkVertexId_ = 1;
		cm::material* rTextureProjectile = componentManager->GetComponent<cm::material>(uiEntity_Projectile);
		*rTextureProjectile = { .diffuseTextureID_ = 2, .specularTextureID_ = 2, .ambient = { 0.05f, 0.05f, 0.0f },
		.shininess = 128.0f * 0.078125f };
        TextureSystem->BindTexture(uiEntity_Projectile, rTextureProjectile->diffuseTextureID_);
        // rTextureProjectile.iWidth_  = 96;
        // rTextureProjectile.iHeight_ = 128;
        // rTextureProjectile.u_iData_ = chelik_dat;
//        core::CEngine::GetInstance()->LoadTextureData(rTextureProjectile);
        cm::transform* rTransformProjectile = componentManager->GetComponent<cm::transform>(uiEntity_Projectile);
        rTransformProjectile->fScale = 0.1f;
//        Vector<float, 3> vec(0.0f);
		
		cm::transform* transform = componentManager->GetComponent<cm::transform>(entityRefMove);
		if ( transform != nullptr )
			rTransformProjectile->tPosition = transform->tPosition;
		
        rTransformProjectile->tForward = GetDirectionVector(beholder);

        rTransformProjectile->tPosition += rTransformProjectile->tForward;
    }
    
    Vector<float, 3> CProjectileSystem::GetDirectionVector(components::beholder& beholder)
    {
        const float kSensitivity = 0.1f;

        fYaw = g_eEvent.mousePointerPosition.offset_X;
        fPitch = g_eEvent.mousePointerPosition.offset_Y;
        fYaw *= kSensitivity;
        fPitch *= kSensitivity;

        g_eEvent.mousePointerPosition.pitch = fPitch;
        g_eEvent.mousePointerPosition.yaw = fYaw;
        
        if(fPitch > 89.0f)
            fPitch = 89.0f;
        if(fPitch < -89.0f)
            fPitch = -89.0f;

        Vector<float, 3> front;
        front[0] = std::cos(Radians(fYaw)) * std::cos(Radians(fPitch));
        front[1] = std::sin(Radians(fPitch));
        front[2] = std::sin(Radians(fYaw)) * std::cos(Radians(fPitch));
        beholder.forward = Normalize(front);

        return beholder.forward;
    }
}
