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
#include "Components/ProjectileBundle.hpp"
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
		namespace cm = GLVM::ecs::components;
		namespace arch = GLVM::ecs::arch;
		
        float cameraSpeed = 5.5f * deltaFrameTime;

		playerArchetypesNumber = 0;
		arch::world.searchCacheArchetypes( playerRequiredMask, &archView.playerCachedArchetype, playerArchetypesNumber );
		componentsView.playerTransforms = (ecs::components::transform*)archView.playerCachedArchetype->
			components[arch::ComponentsIndices::TRANSFORM_COMPONENT];
		componentsView.playerViews      = (ecs::components::beholder*)archView.playerCachedArchetype->
			components[arch::ComponentsIndices::VIEW_COMPONENT];

		ecs::arch::world.searchCacheArchetypes( projectileRequiredMask, &archView.projectileArchetype, projectileArchetypesNumber );
		projectileArchetypesNumber = 0;
		
        if(projectileCooldown > 0)
            projectileCooldown -= cameraSpeed;

		/// Iterate on every player and create projectile if "LMB pressed" event found 
        for(unsigned int i = 0; i < archView.playerCachedArchetype->entityCount; ++i) {
			cm::beholder*  playerView      = &componentsView.playerViews[i];
			cm::transform* playerTransform = &componentsView.playerTransforms[i];
            for(int n = 0; n < 6; ++n) {
                if(!isInventoryOpened && inputStack.SearchElement(core::EEvents::eMOUSE_LEFT_BUTTON) == core::EEvents::eMOUSE_LEFT_BUTTON) {
                    if(projectileCooldown <= 0) {
						ecs::components::MeshHandle meshHandle{};
						const u32 sphereMeshHandleIndex = 2;
						if ( meshHandlers.GetSize() > 2 )
							meshHandle = meshHandlers[sphereMeshHandleIndex];

						ecs::TextureHandle textureHandle{};
						const u32 grayTextureHandle = 2;
						if ( textureHandlers.GetSize() > 2 )
							textureHandle = textureHandlers[grayTextureHandle];

						const components::material material = { .diffuseTextureID_ = textureHandle,
							.specularTextureID_ = textureHandle, .ambient = { 0.05f, 0.05f, 0.05f },
							.shininess = 128.0f * 0.078125f };

						const components::damage damage = { .maximumDamage = 40, .minimumDamage = 20, .criticalHitRate = 0, .criticalModifier = 0 };

						ecs::arch::ArchetypeEntityManager* archEntityManager = ecs::arch::ArchetypeEntityManager::getInstance();
						ecs::arch::entity projectileEntity = archEntityManager->createEntity();
						ecs::arch::world.addEntityToArchetype( projectileEntity, archView.projectileArchetype );
						ecs::arch::EntityLocation projectileLocation = ecs::arch::world.entityLocations[ecs::arch::getId( projectileEntity )];
						
						core::CreateProjectile(playerTransform->position,
											   playerView->forward,
											   meshHandle,
											   material,
											   damage,
											   projectileLocation);

						soundEngine->CreateSoundSample( "../laser2.wav", 5, 22050, 0.05 );
                        projectileCooldown = 2.0;
                    }
                }
            }
        }

		componentsView.projectileTransforms    = (ecs::components::transform*)archView.projectileArchetype->
			components[arch::ComponentsIndices::TRANSFORM_COMPONENT];
		componentsView.projectileColliderFlags = (ecs::components::colliderFlags*)archView.projectileArchetype->
			components[arch::ComponentsIndices::COLLIDER_FLAGS_COMPONENT];
		componentsView.projectileColliders     = (ecs::components::collider*)archView.projectileArchetype->
			components[arch::ComponentsIndices::COLLIDER_COMPONENT];
		componentsView.projectileBundles     = (arch::ProjectileBundle*)archView.projectileArchetype->
			components[arch::ComponentsIndices::PROJECTILE_BUNDLE_COMPONENT];

		/// Update position of every projectile
        for(unsigned int x = 0; x < archView.projectileArchetype->entityCount; ++x) {
            cm::transform* projectileTransform = &componentsView.projectileTransforms[x];
			projectileTransform->position += Normalize(projectileTransform->forward) * cameraSpeed * 2.5;
		}

		/// Iterate every projectile, check for collistions with another entities and update damage info if collided entity has attack component
        for(unsigned int i = 0; i < archView.projectileArchetype->entityCount; ++i) {
			cm::colliderFlags* projectileColliderFlags = &componentsView.projectileColliderFlags[i];
            if((projectileColliderFlags->flags & 1) || (projectileColliderFlags->flags & (1 << 1))) {
				cm::damage* projectileDamage = &componentsView.projectileBundles[i].damage;
				cm::collider* projectileCollider = &componentsView.projectileColliders[i];
				for ( unsigned int j = 0; j < projectileCollider->colliders.GetSize(); ++j ) {
					unsigned int collidedEntity = projectileCollider->colliders[j];

					arch::EntityLocation collidedEntityLocation = arch::world.entityLocations[arch::getId( collidedEntity )];
					arch::componentMask requiredMask = (1ul << arch::ComponentsIndices::HEALTH_COMPONENT) |
						(1ul << arch::ComponentsIndices::ATTACK_COMPONENT);

					if( (collidedEntityLocation.arch->mask & requiredMask) == requiredMask ) {
						ecs::components::attack* attacks = (ecs::components::attack*)collidedEntityLocation.arch->
			components[arch::ComponentsIndices::ATTACK_COMPONENT];
						attacks[collidedEntityLocation.index].damage = projectileDamage->maximumDamage;
					}
				}
//                pEntity_Manager->RemoveEntity(uiEntity_refProjectile, pComponent_Manager);
            }
        }
    }
}
