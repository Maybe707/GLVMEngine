// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Systems/EnemySystem.hpp"
#include "ArchetypeECS/ArchECS_Types.hpp"
#include "Archetypes/EnemyArchetype.hpp"
#include "Archetypes/ProjectileArchetype.hpp"
#include "Components/ActorComponent.hpp"
#include "ArchetypeECS/ArchECS_World.hpp"
#include "Archetypes/PlayerArchetype.hpp"
#include "ArchetypeECS/ArchetypeEntityManager.hpp"
#include "Components/AnimationComponent.hpp"
#include "Components/DamageComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/ProjectileBundle.hpp"
#include "Texture.hpp"
#include <cstdint>

namespace GLVM::ecs
{
	void EnemySystem::Update() {
		namespace arch = GLVM::ecs::arch;

		projectileArchetypesNumber = 0;
		archView.projectileArchetype = world_.findArchetype(projectileRequiredMask);
        projectileArchetypesNumber = archView.projectileArchetype ? 1 : 0;

        if (projectileCooldown > 0) projectileCooldown -= 5.5f * deltaFrameTime;
        for (auto* playerChunk : world_.query(playerRequiredMask)) {
            archView.playerCachedArchetype = playerChunk;
		componentsView.playerTransforms = (ecs::components::transform*)archView.playerCachedArchetype->
			components[arch::ComponentsIndices::TRANSFORM_COMPONENT];

        for (auto* enemyChunk : world_.query(enemyRequiredMask)) {
            archView.enemyCachedArchetype = enemyChunk;
		componentsView.enemyTransforms = (ecs::components::transform*)archView.enemyCachedArchetype->
			components[arch::ComponentsIndices::TRANSFORM_COMPONENT];
		componentsView.enemyStates     = (ecs::components::state*)archView.enemyCachedArchetype->
			components[arch::ComponentsIndices::STATE_COMPONENT];
		componentsView.enemyAnimation  = (ecs::components::animation*)archView.enemyCachedArchetype->
			components[arch::ComponentsIndices::ANIMATION_COMPONENT];
		componentsView.enemies         = (ecs::components::enemy*)archView.enemyCachedArchetype->
			components[arch::ComponentsIndices::ENEMY_COMPONENT];

		for( uint32_t j = 0; j < archView.playerCachedArchetype->entityCount; ++j ) {
			components::transform* playerTransformComponent = &componentsView.playerTransforms[j];
			for ( unsigned int i = 0; i < archView.enemyCachedArchetype->entityCount; ++i ) {
				components::transform* enemyTransformComponent = &componentsView.enemyTransforms[i];
				components::state*     stateEnemyComponent     = &componentsView.enemyStates[i];
				components::animation* enemyAnimatin           = &componentsView.enemyAnimation[i];
				components::enemy*     enemyComponent          = &componentsView.enemies[i];

				enemyAnimatin->isAnimatedOnFrame = true;       ///< FIXME: DELETE CRINGE
				
				vec3 distance = playerTransformComponent->position - enemyTransformComponent->position;

				

				if ( distance.Length() > enemyComponent->detectRadius && stateEnemyComponent->state == core::States::ATTACK ) {
					float deltaLength = distance.Length() - enemyComponent->detectRadius;
					vec3 enemyMove = distance * (deltaLength / distance.Length());

					enemyTransformComponent->position += enemyMove;
				}
			
				if ( distance.Length() <= enemyComponent->detectRadius ) {
					if(archView.projectileArchetype && projectileCooldown <= 0) {
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

						ecs::arch::ArchetypeEntityManager* archEntityManager = &world_.entities;
						ecs::arch::entity projectileEntity = archEntityManager->createEntity();
						world_.addEntityToArchetype( projectileEntity, archView.projectileArchetype );
						ecs::arch::EntityLocation projectileLocation = world_.entityLocations[ecs::arch::getId( projectileEntity )];
						arch::ProjectileArchetype* projectileArch = static_cast<arch::ProjectileArchetype*>(projectileLocation.arch);
						const u32 projectileIndex = projectileLocation.index;
						ecs::components::health& projectileHealth = projectileArch->heath[projectileIndex];
						projectileHealth.randarable = false;
						
						core::CreateProjectile(enemyTransformComponent->position,
											   playerTransformComponent->position - enemyTransformComponent->position,
											   meshHandle,
											   material,
											   damage,
											   projectileLocation);

						if (soundEngine) soundEngine->CreateSoundSample( "../laser2.wav", 5, 22050, 0.05 );
						projectileCooldown = 15.0;
					}

					stateEnemyComponent->state = core::States::ATTACK;
				}
			}
		}
        }
        }
	}
} // namespace GLVM::ecs
