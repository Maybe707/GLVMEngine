// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "ArchetypeECS/ArchECS_Types.hpp"
#include "ArchetypeECS/ArchECS_Utils.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"
#include "Archetypes/EnemyArchetype.hpp"
#include "Components/ActorComponent.hpp"
#include "Components/AttackComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ColliderFlagsComponent.hpp"
#include "Components/ControllerComponent.hpp"
#include "Components/DamageComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/PointLightComponent.hpp"
#include "Components/ProjectileComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Texture.hpp"
#include "VertexMath.hpp"
#include <Systems/ProjectileSystem.hpp>
#include <cstdint>
#include "ArchetypeECS/ArchECS_World.hpp"
#include "Archetypes/PlayerArchetype.hpp"
#include "Archetypes/ProjectileArchetype.hpp"
#include "ArchetypeECS/ArchetypeEntityManager.hpp"

namespace GLVM::ecs
{
    CProjectileSystem::CProjectileSystem(core::CStack& inputStack) : inputStack (inputStack)
    {}
    
    void CProjectileSystem::Update()
    {
        // ComponentManager* pComponent_Manager = GLVM::ecs::ComponentManager::GetInstance();
        // EntityManager* pEntity_Manager       = GLVM::ecs::EntityManager::GetInstance();
    
        // core::vector<unsigned int>* pEntity_Container_refMove =
		// 	pComponent_Manager->GetEntityContainer<cm::controller>();
        // unsigned int u_iVector_Move_Size = pEntity_Container_refMove->GetSize();

        // core::vector<unsigned int>* pEntity_Container_refView =
		// 	pComponent_Manager->GetEntityContainer<cm::beholder>();

		// unsigned int iEntity_refView = 0;
		// if ( pEntity_Container_refView->GetSize() > 0 )
		// 	iEntity_refView = (*pEntity_Container_refView)[0];
		namespace cm = GLVM::ecs::components;
		namespace arch = GLVM::ecs::arch;
		
        float cameraSpeed = 5.5f * deltaFrameTime;
		arch::PlayerArchetype* playerArch = {};
		uint32_t playerEntityCount = 0;
		if( arch::world.archetypes.GetSize() > 1 ) {
			playerArch = static_cast<arch::PlayerArchetype*>(arch::world.archetypes[1]);
			playerEntityCount = playerArch->entityCount;
		}
        if(projectileCooldown > 0)
            projectileCooldown -= cameraSpeed;

        for(unsigned int i = 0; i < playerEntityCount; ++i) {
			cm::beholder* viewComponent = &playerArch->beholders[i];
			cm::transform* playerTransform = &playerArch->transforms[i];
            for(int n = 0; n < 6; ++n) {
                if(!isInventoryOpened && inputStack.SearchElement(core::EEvents::eMOUSE_LEFT_BUTTON) == core::EEvents::eMOUSE_LEFT_BUTTON) {
                    if(projectileCooldown <= 0) {
                        CalculateProjectile(playerTransform,
                                            viewComponent);
                        projectileCooldown = 2.0;
                    }
                }
            }
        }

		arch::ProjectileArchetype* projectileArch = {};
		uint32_t projectileEntityCount = 0;
		if( arch::world.archetypes.GetSize() > 3 ) {
			projectileArch = static_cast<arch::ProjectileArchetype*>(arch::world.archetypes[3]);
			projectileEntityCount = projectileArch->entityCount;
		}
        for(unsigned int x = 0; x < projectileEntityCount; ++x) {
            cm::transform* projectileTransform = &projectileArch->transforms[x];
			projectileTransform->position += Normalize(projectileTransform->forward) * cameraSpeed * 0.5;
		}

        for(unsigned int i = 0; i < projectileEntityCount; ++i) {
			cm::colliderFlags* projectileColliderFlags = &projectileArch->colliderFlags[i];
            if((projectileColliderFlags->flags & 1) || (projectileColliderFlags->flags & (1 << 1))) {
				cm::damage* projectileDamage = &projectileArch->projectileBundles[i].damage;
				cm::collider* projectileCollider = &projectileArch->colliders[i];
				for ( unsigned int j = 0; j < projectileCollider->colliders.GetSize(); ++j ) {
					unsigned int collidedEntity = projectileCollider->colliders[j];

					arch::EntityLocation collidedEntityLocation = arch::world.entityLocations[arch::getId( collidedEntity )];
					arch::componentMask requiredMask = (1ul << arch::ComponentsIndices::HEALTH_COMPONENT) |
						(1ul << arch::ComponentsIndices::ATTACK_COMPONENT);

					if( (collidedEntityLocation.arch->mask & requiredMask) == requiredMask ) {
						switch( collidedEntityLocation.arch->mask ) {
						case arch::playerComponentMask:
							markAsAttacked( static_cast<arch::PlayerArchetype*>( collidedEntityLocation.arch ), projectileDamage,
													collidedEntityLocation.index );

							
							static_cast<arch::PlayerArchetype*>(collidedEntityLocation.arch)->attacks[collidedEntityLocation.index].damage = projectileDamage->maximumDamage;
							break;
						case arch::enemyComponentMask:
							markAsAttacked( static_cast<arch::EnemyArchetype*>( collidedEntityLocation.arch ), projectileDamage,
													collidedEntityLocation.index );

							static_cast<arch::EnemyArchetype*>(collidedEntityLocation.arch)->attacks[collidedEntityLocation.index].damage = projectileDamage->maximumDamage;
							break;
						}
					}
				}
//                pEntity_Manager->RemoveEntity(uiEntity_refProjectile, pComponent_Manager);
            }
        }
    }

    void CProjectileSystem::CalculateProjectile(components::transform* playerTransform,
												components::beholder* beholder) {
		namespace cm = GLVM::ecs::components;

		arch::ArchetypeEntityManager* archEntityManager = arch::ArchetypeEntityManager::getInstance();
		arch::entity projectileEntity = archEntityManager->createEntity();
		arch::world.addEntityToArchetype( projectileEntity, arch::world.archetypes[3] );
		arch::EntityLocation projectileLocation = arch::world.entityLocations[arch::getId( projectileEntity )];
		arch::ProjectileArchetype* projectileArch = static_cast<arch::ProjectileArchetype*>(projectileLocation.arch);
		const uint32_t projectileIndex = projectileLocation.index;
		
        core::Sound::CSoundSample* pSound_Sample = new core::Sound::CSoundSample();
        pSound_Sample->kPath_to_File_ = "../laser2.wav";
        pSound_Sample->uiDuration_ = 5;
        pSound_Sample->uiRate_ = 22050;
        soundEngine->GetSoundContainer().Push(pSound_Sample);

		ecs::components::MeshHandle meshHandle{};
		if ( meshHandlers.GetSize() > 0 )
			meshHandle = meshHandlers[0];
		
		projectileArch->meshes[projectileIndex].handle = meshHandle;
		ecs::TextureHandle textureHandle{};
		if ( textureHandlers.GetSize() > 0 )
			textureHandle = textureHandlers[0];
		
		projectileArch->projectileBundles[projectileIndex].material  = { .diffuseTextureID_ = textureHandle,
			.specularTextureID_ = textureHandle, .ambient = { 0.05f, 0.05f, 0.05f },
			.shininess = 128.0f * 0.078125f };
		components::transform* rTransformProjectile = &projectileArch->transforms[projectileIndex];
		rTransformProjectile->scale = 0.1f;
		
		cm::transform* transform = playerTransform;
		if ( transform != nullptr )
			rTransformProjectile->position = transform->position;

        rTransformProjectile->forward   = beholder->forward;
//		rTransformProjectile->yaw        = fYaw;
//		rTransformProjectile->pitch      = fPitch;
		rTransformProjectile->position  += rTransformProjectile->forward * 2.0f;

		components::damage* damageComponent = &projectileArch->projectileBundles[projectileIndex].damage;
		damageComponent->maximumDamage = 40;
		damageComponent->minimumDamage = 20;
    }
}
