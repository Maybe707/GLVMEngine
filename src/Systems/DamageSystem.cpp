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
			fontComponent->font_string.Push('4');
			fontComponent->font_string.Push('0');

		}
	}
} // namespace GLVM::ecs
