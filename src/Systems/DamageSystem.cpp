// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Systems/DamageSystem.hpp"
#include "Components/AttackComponent.hpp"
#include "Components/FontComponent.hpp"
#include "ArchetypeECS/ArchECS_World.hpp"
#include "Archetypes/PlayerArchetype.hpp"
#include "Archetypes/ProjectileArchetype.hpp"
#include "Archetypes/StaticMeshArchetype.hpp"
#include "Archetypes/EnemyArchetype.hpp"

namespace GLVM::ecs
{
	void DamageSystem::Update() {
		namespace cm = GLVM::ecs::components;

		cachedArchetypesNumber = 0;
		for( uint32_t i = 0; i < arch::world.archetypes.GetSize(); ++i ) {
			arch::Archetype* arch = arch::world.archetypes[i];
			arch::componentMask requiredMask = (1ul << arch::ComponentsIndices::ATTACK_COMPONENT) |
				(1ul << arch::ComponentsIndices::HEALTH_COMPONENT) |
				(1ul << arch::ComponentsIndices::FONT_COMPONENT);

			if( (arch->mask & requiredMask) == requiredMask ) {
				cachedArchetypes[cachedArchetypesNumber] = arch;
				++cachedArchetypesNumber;
			}
		}

		cachedFontArchetypesNumber = 0;
		for( uint32_t x = 0; x < cachedArchetypesNumber; ++x ) {
			arch::Archetype* arch = cachedArchetypes[x];
			components::attack* attackView = nullptr;
			components::health* healthView = nullptr;
			components::font*   fontView   = nullptr;
			switch( arch->mask ) {
			case arch::playerComponentMask:
				attackView = static_cast<arch::PlayerArchetype*>( arch )->attacks;
				healthView = static_cast<arch::PlayerArchetype*>( arch )->health;
				fontView   = static_cast<arch::PlayerArchetype*>( arch )->fonts;
				break;
			case arch::enemyComponentMask:
				attackView = static_cast<arch::EnemyArchetype*>( arch )->attacks;
				healthView = static_cast<arch::EnemyArchetype*>( arch )->health;
				fontView   = static_cast<arch::EnemyArchetype*>( arch )->fonts;
				break;
			}

			for ( unsigned int i = 0; i < arch->entityCount; ++i ) {
//				unsigned int entity = linkedEntities[i];
				if( &healthView[i] != nullptr && &attackView[i] != nullptr ) {
					cm::health& healthComponent = healthView[i];
					cm::attack& attackComponent = attackView[i];
				
					healthComponent.currentHealth -= attackComponent.damage;
//					std::cout << "current health: " << healthComponent.currentHealth << std::endl;
//				componentManager->RemoveComponent<cm::attack>(entity);
					if ( healthComponent.currentHealth <= 0 ) {
//					std::cout << "remove entity: " << entity << std::endl;
//					entityManager->RemoveEntity(entity, componentManager);
					}

					cm::font& fontComponent = fontView[i];
					fontComponent.font_string.clear();
					fontComponent.font_string.Push('4');
					fontComponent.font_string.Push('0');
					fontComponent.lifeTime = 0;
					fontComponent.removeble = true;
				}
			}
		}

		for( uint32_t i = 0; i < arch::world.archetypes.GetSize(); ++i ) {
			arch::Archetype* arch = arch::world.archetypes[i];
			arch::componentMask requiredMask = (1ul << arch::ComponentsIndices::FONT_COMPONENT);

			if( (arch->mask & requiredMask) == requiredMask ) {
				cachedFontArchetypes[cachedFontArchetypesNumber] = arch;
				++cachedFontArchetypesNumber;
			}
		}

		for( uint32_t x = 0; x < cachedFontArchetypesNumber; ++x ) {
			arch::Archetype* arch = cachedFontArchetypes[x];
			components::font* fontView = nullptr;
			switch( arch->mask ) {
			case arch::enemyComponentMask:
				fontView = static_cast<arch::EnemyArchetype*>( arch )->fonts;
				break;
			}

			for ( unsigned int i = 0; i < arch->entityCount; ++i ) {
				if( fontView ) {
					cm::font& fontComponent = fontView[i];
					if ( fontComponent.removeble )
						fontComponent.lifeTime += deltaTime;
//			std::cout << "lifeTime" << fontComponent->lifeTime << std::endl;
					if ( fontComponent.lifeTime >= 1.5 ) {
//				std::cout << "lifetime: " << fontComponent->lifeTime << std::endl;
//					componentManager->RemoveComponent<cm::font>(entity);
					}
				}
			}
		}
	}
} // namespace GLVM::ecs
