// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Systems/EnemySystem.hpp"

namespace GLVM::ecs
{
	void EnemySystem::Update() {
		namespace cm = GLVM::ecs::components;
		
        ComponentManager* componentManager = GLVM::ecs::ComponentManager::GetInstance();

		core::vector<unsigned int>* entityContainerRefController =
			componentManager->GetEntityContainer<cm::controller>();
		unsigned int playerEntity = (*entityContainerRefController)[0];
		cm::transform* playerTransformComponent = componentManager->GetComponent<cm::transform>(playerEntity);
		
		core::vector<Entity> linkedEntities      = componentManager->collectLinkedEntities<cm::enemy, cm::transform, cm::state>();

		for ( unsigned int i = 0; i < linkedEntities.GetSize(); ++i ) {
			unsigned int enemyEntity = linkedEntities[i];
			cm::transform* enemyTransformComponent = componentManager->GetComponent<cm::transform>(enemyEntity);
            cm::state* stateEnemyComponent = componentManager->GetComponent<cm::state>(enemyEntity);
			cm::enemy* enemyComponent = componentManager->GetComponent<cm::enemy>(enemyEntity);

			vec3 distance = playerTransformComponent->tPosition - enemyTransformComponent->tPosition;
			float cameraSpeed = 5.5f * deltaFrameTime;            


			if(projectileCooldown > 0)
				projectileCooldown -= cameraSpeed;
//			std::cout << distance << std::endl;
			if ( distance.Length() > enemyComponent->detectRadius && stateEnemyComponent->state == core::States::ATTACK ) {
 				// if(projectileCooldown <= 0) {
				// 	CalculateProjectile(playerTransformComponent, enemyTransformComponent);
				// 	projectileCooldown = 5.0;
				// }

				float deltaLenth = distance.Length() - enemyComponent->detectRadius;
				vec3 enemyMove = distance * (deltaLenth / distance.Length());

				enemyTransformComponent->tPosition += enemyMove;
			}
			
			if ( distance.Length() <= enemyComponent->detectRadius ) {
 				if(projectileCooldown <= 0) {
					CalculateProjectile(playerEntity, enemyEntity);
					projectileCooldown = 5.0;
				}

				stateEnemyComponent->state = core::States::ATTACK;
			}
		}
	}

	void EnemySystem::CalculateProjectile(unsigned int playerEntity, unsigned int enemyEntity) {
		ComponentManager* componentManager = GLVM::ecs::ComponentManager::GetInstance();
		namespace cm = GLVM::ecs::components;

		unsigned int uiEntity_Projectile = ecs::EntityManager::GetInstance()->CreateEntity();
		ecs::ComponentManager::GetInstance()->CreateComponent<cm::mesh, cm::collider,
															  cm::transform, cm::material,
															  cm::projectile, cm::pointLight,
															  cm::damage>(uiEntity_Projectile);

		core::Sound::CSoundSample* pSound_Sample = new core::Sound::CSoundSample();
		pSound_Sample->kPath_to_File_ = "../laser2.wav";
		pSound_Sample->uiDuration_ = 5;
		pSound_Sample->uiRate_ = 22050;
		soundEngine->GetSoundContainer().Push(pSound_Sample);

		ecs::components::MeshHandle meshHandle{};
		if ( meshHandlers.GetSize() > 0 )
			meshHandle = meshHandlers[0];
		componentManager->GetComponent<cm::mesh>(uiEntity_Projectile)->handle = meshHandle;
		ecs::TextureHandle textureHandle{};
		if ( textureHandlers.GetSize() > 0 )
			textureHandle = textureHandlers[0];
		cm::material* rTextureProjectile = componentManager->GetComponent<cm::material>(uiEntity_Projectile);
		*rTextureProjectile = { .diffuseTextureID_ = textureHandle, .specularTextureID_ = textureHandle, .ambient = { 0.05f, 0.05f, 0.05f },
			.shininess = 128.0f * 0.078125f };
		cm::transform* rTransformProjectile = componentManager->GetComponent<cm::transform>(uiEntity_Projectile);
		rTransformProjectile->fScale = 0.1f;

		cm::transform* playerTransformComponent = componentManager->GetComponent<cm::transform>(playerEntity);
		cm::transform* enemyTransformComponent = componentManager->GetComponent<cm::transform>(enemyEntity);
		
		rTransformProjectile->tPosition = enemyTransformComponent->tPosition;
		rTransformProjectile->tForward   = playerTransformComponent->tPosition - enemyTransformComponent->tPosition;
		rTransformProjectile->tPosition += rTransformProjectile->tForward * 0.3;
		
		*(componentManager->GetComponent<cm::pointLight>(uiEntity_Projectile)) = { .position = rTransformProjectile->tPosition,
			.ambient = { 0.1f, 0.1f, 0.1f }, .diffuse = { 0.5f, 0.5f, 0.5f }, .specular = { 1.1f, 1.2f, 1.3f },
			.constant = 1.4f, .linear = 0.1f, .quadratic = 0.128f };

		cm::damage* damageComponent = componentManager->GetComponent<cm::damage>(uiEntity_Projectile);
		damageComponent->maximumDamage = 40;
		damageComponent->minimumDamage = 20;
	}
} // namespace GLVM::ecs
