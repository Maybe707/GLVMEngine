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

namespace GLVM::ecs
{
	void EnemySystem::Update() {
		namespace arch = GLVM::ecs::arch;
		
		arch::PlayerArchetype* playerArch = static_cast<arch::PlayerArchetype*>(arch::world.archetypes[1]);
		arch::EnemyArchetype*  enemyArch  = static_cast<arch::EnemyArchetype*>(arch::world.archetypes[2]);
		for( uint32_t j = 0; j < playerArch->entityCount; ++j ) {
			components::transform* playerTransformComponent = &playerArch->transforms[j];
			for ( unsigned int i = 0; i < enemyArch->entityCount; ++i ) {
				components::transform* enemyTransformComponent = &enemyArch->transforms[i];
				components::state* stateEnemyComponent         = &enemyArch->states[i];
				components::enemy* enemyComponent              = &enemyArch->enemies[i];

				vec3 distance = playerTransformComponent->position - enemyTransformComponent->position;
			
				float cameraSpeed = 5.5f * deltaFrameTime;            


				if(projectileCooldown > 0)
					projectileCooldown -= cameraSpeed;
				if ( distance.Length() > enemyComponent->detectRadius && stateEnemyComponent->state == core::States::ATTACK ) {
					float deltaLenth = distance.Length() - enemyComponent->detectRadius;
					vec3 enemyMove = distance * (deltaLenth / distance.Length());

					enemyTransformComponent->position += enemyMove;
				}
			
				if ( distance.Length() <= enemyComponent->detectRadius ) {
					if(projectileCooldown <= 0) {
						CalculateProjectile(playerTransformComponent, enemyTransformComponent);
						projectileCooldown = 5.0;
					}

					stateEnemyComponent->state = core::States::ATTACK;
				}
			}
		}
	}

	void EnemySystem::CalculateProjectile(components::transform* playerTransformComponent, components::transform* enemyTransformComponent) {
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

		rTransformProjectile->position = enemyTransformComponent->position;
		rTransformProjectile->forward   = playerTransformComponent->position - enemyTransformComponent->position;
		rTransformProjectile->position += rTransformProjectile->forward * 0.3;
		
		// *(componentManager->GetComponent<cm::pointLight>(uiEntity_Projectile)) = { .position = rTransformProjectile->position,
		// 	.ambient = { 0.1f, 0.1f, 0.1f }, .diffuse = { 0.5f, 0.5f, 0.5f }, .specular = { 1.1f, 1.2f, 1.3f },
		// 	.constant = 1.4f, .linear = 0.1f, .quadratic = 0.128f };

		components::damage* damageComponent = &projectileArch->projectileBundles[projectileIndex].damage;
		damageComponent->maximumDamage = 40;
		damageComponent->minimumDamage = 20;
	}
} // namespace GLVM::ecs
