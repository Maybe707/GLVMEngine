// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Systems/DamageSystem.hpp"
#include "Components/FontComponent.hpp"

namespace GLVM::ecs
{
	void DamageSystem::Update() {
		namespace cm = GLVM::ecs::components;
		
        ComponentManager* componentManager = GLVM::ecs::ComponentManager::GetInstance();
        EntityManager* entityManager       = GLVM::ecs::EntityManager::GetInstance();

		core::vector<Entity> linkedEntities = componentManager->collectLinkedEntities<cm::attack>();

		for ( unsigned int i = 0; i < linkedEntities.GetSize(); ++i ) {
			unsigned int entity = linkedEntities[i];
			cm::health* healthComponent = componentManager->GetComponent<cm::health>(entity);
			cm::attack* attackComponent = componentManager->GetComponent<cm::attack>(entity);

			healthComponent->currentHealth -= attackComponent->damage;
			std::cout << "current health: " << healthComponent->currentHealth << " entity: " << entity << std::endl;
			componentManager->RemoveComponent<cm::attack>(entity);
			if ( healthComponent->currentHealth <= 0 ) {
				std::cout << "remove entity: " << entity << std::endl;
				entityManager->RemoveEntity(entity, componentManager);
			}

			componentManager->CreateComponent<cm::font>(entity);
			cm::font* fontComponent = componentManager->GetComponent<cm::font>(entity);
			fontComponent->font_string.clear();
			fontComponent->font_string.Push('4');
			fontComponent->font_string.Push('0');
			fontComponent->lifeTime = 0;
			fontComponent->removeble = true;
		}

		core::vector<Entity> linkedEntitiesFont = componentManager->collectLinkedEntities<cm::font>();
		for ( unsigned int i = 0; i < linkedEntitiesFont.GetSize(); ++i ) {
			unsigned int entity = linkedEntitiesFont[i];

			cm::font* fontComponent = componentManager->GetComponent<cm::font>(entity);
			if ( fontComponent->removeble )
				fontComponent->lifeTime += deltaTime;
//			std::cout << "lifeTime" << fontComponent->lifeTime << std::endl;
			if ( fontComponent->lifeTime >= 1.5 ) {
//				std::cout << "lifetime: " << fontComponent->lifeTime << std::endl;
				componentManager->RemoveComponent<cm::font>(entity);
			}
		}
	}
} // namespace GLVM::ecs
