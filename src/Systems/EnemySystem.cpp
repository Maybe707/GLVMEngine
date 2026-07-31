// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Systems/EnemySystem.hpp"
#include "Archetypes/EnemyArchetype.hpp"
#include "Archetypes/ProjectileArchetype.hpp"
#include "Components/ActorComponent.hpp"
#include "ArchetypeECS/ArchECS_World.hpp"
#include "Archetypes/PlayerArchetype.hpp"
#include "ArchetypeECS/ArchetypeEntityManager.hpp"
#include "Components/DamageComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/ProjectileBundle.hpp"
#include "Texture.hpp"
#include <cstdint>

namespace GLVM::ecs
{
	void EnemySystem::Update() {
		namespace arch = GLVM::ecs::arch;

		playerArchetypesNumber = 0;
		arch::world.searchCacheArchetypes( playerRequiredMask, &archView.playerCachedArchetype, playerArchetypesNumber );
		componentsView.playerTransforms = (ecs::components::transform*)archView.playerCachedArchetype->
			components[arch::ComponentsIndices::TRANSFORM_COMPONENT];

		enemyArchetypesNumber = 0;
		arch::world.searchCacheArchetypes( enemyRequiredMask, &archView.enemyCachedArchetype, enemyArchetypesNumber );
		componentsView.enemyTransforms = (ecs::components::transform*)archView.enemyCachedArchetype->
			components[arch::ComponentsIndices::TRANSFORM_COMPONENT];
		componentsView.enemyStates     = (ecs::components::state*)archView.enemyCachedArchetype->
			components[arch::ComponentsIndices::STATE_COMPONENT];
		componentsView.enemies         = (ecs::components::enemy*)archView.enemyCachedArchetype->
			components[arch::ComponentsIndices::ENEMY_COMPONENT];
		
		for( uint32_t j = 0; j < archView.playerCachedArchetype->entityCount; ++j ) {
			components::transform* playerTransformComponent = &componentsView.playerTransforms[j];
			for ( unsigned int i = 0; i < archView.enemyCachedArchetype->entityCount; ++i ) {
				components::transform* enemyTransformComponent = &componentsView.enemyTransforms[i];
				components::state*     stateEnemyComponent     = &componentsView.enemyStates[i];
				components::enemy*     enemyComponent          = &componentsView.enemies[i];

				vec3 distance = playerTransformComponent->position - enemyTransformComponent->position;
				float cameraSpeed = 5.5f * deltaFrameTime;
				
				if(projectileCooldown > 0)
					projectileCooldown -= cameraSpeed;
				if ( distance.Length() > enemyComponent->detectRadius && stateEnemyComponent->state == core::States::ATTACK ) {
					float deltaLength = distance.Length() - enemyComponent->detectRadius;
					vec3 enemyMove = distance * (deltaLength / distance.Length());

					enemyTransformComponent->position += enemyMove;
				}
			
				if ( distance.Length() <= enemyComponent->detectRadius ) {
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
						
						CalculateProjectile(enemyTransformComponent->position,
											playerTransformComponent->position - enemyTransformComponent->position,
											meshHandle,
											material,
											damage);

						// core::Sound::CSoundSample* pSound_Sample = new core::Sound::CSoundSample();
						// pSound_Sample->kPath_to_File_ = "../laser2.wav";
						// pSound_Sample->uiDuration_ = 5;
						// pSound_Sample->uiRate_ = 22050;
						// pSound_Sample->volume  = 0.05;
						// soundEngine->GetSoundContainer().Push(pSound_Sample);

						soundEngine->CreateSoundSample( "../laser2.wav", 5, 22050, 0.05 );
						
						projectileCooldown = 5.0;
					}

					stateEnemyComponent->state = core::States::ATTACK;
				}
			}
		}
	}

	void EnemySystem::CalculateProjectile(const vec3& projectilePosition,
										  const vec3& projectileForward,
										  const ecs::components::MeshHandle& meshHandle,
										  const components::material& material,
										  const components::damage& damage) {
		arch::ArchetypeEntityManager* archEntityManager = arch::ArchetypeEntityManager::getInstance();
		arch::entity projectileEntity = archEntityManager->createEntity();
		projectileArchetypesNumber = 0;
		arch::world.searchCacheArchetypes( projectileRequiredMask, &archView.projectileArchetype, projectileArchetypesNumber );
		arch::world.addEntityToArchetype( projectileEntity, archView.projectileArchetype );
		arch::EntityLocation projectileLocation = arch::world.entityLocations[arch::getId( projectileEntity )];
		arch::ProjectileArchetype* projectileArch = static_cast<arch::ProjectileArchetype*>(projectileLocation.arch);
		const uint32_t projectileIndex = projectileLocation.index;
		
		// core::Sound::CSoundSample* pSound_Sample = new core::Sound::CSoundSample();
		// pSound_Sample->kPath_to_File_ = "../laser2.wav";
		// pSound_Sample->uiDuration_ = 5;
		// pSound_Sample->uiRate_ = 22050;
		// soundEngine->GetSoundContainer().Push(pSound_Sample);

		ecs::components::mesh* projectileMesh = &projectileArch->meshes[projectileIndex];
		projectileMesh->handle = meshHandle;

		arch::ProjectileBundle* projectileBundle = &projectileArch->projectileBundles[projectileIndex];
		projectileBundle->material  = material;
		
		components::transform* rTransformProjectile = &projectileArch->transforms[projectileIndex];
		rTransformProjectile->scale = 0.1f;
		rTransformProjectile->position = projectilePosition;
		rTransformProjectile->forward   = projectileForward;
		rTransformProjectile->position += rTransformProjectile->forward * 0.3;
		
		projectileBundle->damage = damage;
	}
} // namespace GLVM::ecs
