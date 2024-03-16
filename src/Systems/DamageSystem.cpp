#include "Systems/DamageSystem.hpp"

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
			std::cout << "current health: " << healthComponent->currentHealth << std::endl;
			componentManager->RemoveComponent<cm::attack>(entity);
			if ( healthComponent->currentHealth <= 0 )
				entityManager->RemoveEntity(entity, componentManager);
		}
	}
} // namespace GLVM::ecs
