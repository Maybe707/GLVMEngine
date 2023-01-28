#include "ComponentManager.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ProjectileComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "EntityManager.hpp"
#include <Systems/ProjectileSystem.hpp>
#include "Stack.hpp"

namespace GLVM::ECS
{
    CProjectileSystem::CProjectileSystem(Core::CStack& _input_Stack) : Input_Stack_ (_input_Stack)
    {}
    
    void CProjectileSystem::Update()
    {
        CComponentManager* pComponent_Manager = GLVM::ECS::CComponentManager::GetInstance();
        CEntityManager* pEntity_Manager       = GLVM::ECS::CEntityManager::GetInstance();
    
        Core::TCVectorContainer<unsigned int>* pEntity_Container_refMove =
            ECS::GetInnerIDsContainer<ECS::move>(*pComponent_Manager);
        unsigned int u_iVector_Move_Size = pEntity_Container_refMove->GetSize();

        Core::TCVectorContainer<unsigned int>* pEntity_Container_refView =
            ECS::GetInnerIDsContainer<ECS::beholder>(*pComponent_Manager);
        unsigned int iEntity_refView = (*pEntity_Container_refView)[0];
        ECS::beholder& view_Component = pComponent_Manager->GetComponent<ECS::beholder>(iEntity_refView);
        
        float cameraSpeed = 5.5f * _dOffset;            

        if(fProjectile_Accumulator_ > 0)
            fProjectile_Accumulator_ -= cameraSpeed;

        for(unsigned int i = 0; i < u_iVector_Move_Size; ++i) {
            for(int n = 0; n < 6; ++n) {
                unsigned int iEntity_refMove = (*pEntity_Container_refMove)[i];
                
                if(Input_Stack_.SearchElement(Core::EEvents::eMOUSE_LEFT_BUTTON) == Core::EEvents::eMOUSE_LEFT_BUTTON) {
                    if(fProjectile_Accumulator_ <= 0) {
                        CalculateProjectile(pComponent_Manager,
                                            iEntity_refMove,
                                            view_Component);
                        fProjectile_Accumulator_ = 2.0;
                    }
                }
            }
        }

        Core::TCVectorContainer<unsigned int>* pEntity_Container_refProjectile =
            GetInnerIDsContainer<CProjectileComponent>(*pComponent_Manager);
        unsigned int uiVector_Projectile_Size = pEntity_Container_refProjectile->GetSize();

        for(unsigned int x = 0; x < uiVector_Projectile_Size; ++x) {
            unsigned int uiEntity_refProjectile = (*pEntity_Container_refProjectile)[x];
            ECS::STransformComponent& rTransformProjectile = pComponent_Manager->GetComponent<ECS::STransformComponent>(uiEntity_refProjectile);
            rTransformProjectile.tPosition += rTransformProjectile.tForward * 0.2f;
        }

        GLVM::ECS::CTextureManager*    TextureSystem    = GLVM::ECS::CTextureManager::GetInstance();

        
        for(unsigned int i = 0; i < uiVector_Projectile_Size; ++i) {
            unsigned int uiEntity_refProjectile = (*pEntity_Container_refProjectile)[i];
            
            if(pComponent_Manager->GetComponent<collider>(uiEntity_refProjectile).bWall_Collision_ ||
               pComponent_Manager->GetComponent<collider>(uiEntity_refProjectile).bGround_Collision_) {
//                std::cout << "FLAG: " << pComponent_Manager->GetComponent<collider>(uiEntity_refProjectile).bWall_Collision_ << std::endl;
                pEntity_Manager->RemoveEntity(uiEntity_refProjectile, pComponent_Manager);
                GLVM::ECS::material& textureProjectile = pComponent_Manager->GetComponent<GLVM::ECS::material>(uiEntity_refProjectile);                      TextureSystem->UnbindTexture(textureProjectile, uiEntity_refProjectile);     
                continue;
            }
        }
    }

    void CProjectileSystem::CalculateProjectile(ECS::CComponentManager* pComponent_Manager,
                                              unsigned int iEntity_refMove,
                                              beholder& view_Component) {
        GLVM::ECS::CTextureManager*    TextureSystem    = GLVM::ECS::CTextureManager::GetInstance();
        
        unsigned int uiEntity_Projectile = ECS::CEntityManager::GetInstance()->CreateEntity();
        ECS::CComponentManager::GetInstance()->CreateComponent<ECS::vertex, ECS::collider,
                                                               ECS::STransformComponent, ECS::material,
                                                               ECS::CProjectileComponent>(uiEntity_Projectile);

        // Core::Sound::CSoundSample* pSound_Sample = new Core::Sound::CSoundSample();
        // pSound_Sample->kPath_to_File_ = "../laser2.wav";
        // pSound_Sample->uiDuration_ = 5;
        // pSound_Sample->uiRate_ = 22050;
        // Sound_Engine_->GetSoundContainer().Push(pSound_Sample);
        
		ECS::material& rTextureProjectile = pComponent_Manager->GetComponent<ECS::material>(uiEntity_Projectile);
        rTextureProjectile.diffuseTextureID_ = 1;
        TextureSystem->BindTexture(uiEntity_Projectile, rTextureProjectile.diffuseTextureID_);
        // rTextureProjectile.iWidth_  = 96;
        // rTextureProjectile.iHeight_ = 128;
        // rTextureProjectile.u_iData_ = chelik_dat;
//        Core::CEngine::GetInstance()->LoadTextureData(rTextureProjectile);
        ECS::STransformComponent& rTransformProjectile = pComponent_Manager->GetComponent<ECS::STransformComponent>(uiEntity_Projectile);
        rTransformProjectile.fScale = 0.2f;
        Vector<float, 3> vec(0.0f);
        rTransformProjectile.tPosition = pComponent_Manager->GetComponent<ECS::STransformComponent>(iEntity_refMove).tPosition;
        rTransformProjectile.tForward = GetDirectionVector(view_Component);

        rTransformProjectile.tPosition +=rTransformProjectile.tForward;
    }
    
    Vector<float, 3> CProjectileSystem::GetDirectionVector(ECS::beholder& _view_Component)
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
