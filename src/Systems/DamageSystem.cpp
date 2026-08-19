// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Systems/DamageSystem.hpp"
#include "ArchetypeECS/ArchECS_Types.hpp"
#include "ArchetypeECS/ArchetypeEntityManager.hpp"
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

		arch::SpatialGrid& spatialGrid = arch::world.spatialGrid;
		
		cachedAttackableArchetypesNumber = 0;
		arch::world.searchCacheArchetypes( attackableRequiredMask, archView.cachedAttackableArchetypes, cachedAttackableArchetypesNumber );

		for( uint32_t x = 0; x < cachedAttackableArchetypesNumber; ++x ) {
			arch::Archetype* arch = archView.cachedAttackableArchetypes[x];
			componentsView.attackableAttacks = (ecs::components::attack*)arch->components[arch::ComponentsIndices::ATTACK_COMPONENT];
			componentsView.attackableHealth  = (ecs::components::health*)arch->components[arch::ComponentsIndices::HEALTH_COMPONENT];
			componentsView.attackableFonts   = (ecs::components::font*)arch->components[arch::ComponentsIndices::FONT_COMPONENT];
			
			for ( unsigned int i = 0; i < arch->entityCount; ++i ) {
//				unsigned int entity = linkedEntities[i];
				arch::entity entity = arch->entities[i];
				if( &componentsView.attackableHealth[i] != nullptr && &componentsView.attackableAttacks[i] != nullptr ) {
					cm::health& healthComponent = componentsView.attackableHealth[i];
					cm::attack& attackComponent = componentsView.attackableAttacks[i];

//					std::cout << "damage: " << attackComponent.damage << std::endl;
					healthComponent.currentHealth -= attackComponent.damage;
					attackComponent.damage = 0;
//					std::cout << "current health: " << healthComponent.currentHealth << std::endl;
//				componentManager->RemoveComponent<cm::attack>(entity);
					if ( healthComponent.currentHealth <= 0 ) {
//					std::cout << "remove entity: " << entity << std::endl;
//					entityManager->RemoveEntity(entity, componentManager);
						ecs::arch::ArchetypeEntityManager* archEntityManager = ecs::arch::ArchetypeEntityManager::getInstance();
						ecs::arch::EntityLocation& entityLocation = ecs::arch::world.entityLocations[ecs::arch::getId( entity )];
						if( entityLocation.gridCellCounter > 0 ) {
							for( u32 i2 = 0; i2 < entityLocation.maxGridCellNumber; ++i2 ) {
								const vec3 entityCurrentGridCell = entityLocation.gridCellIndicies[i2];
								u32 z = entityCurrentGridCell[0];
								u32 y = entityCurrentGridCell[1];
								u32 x = entityCurrentGridCell[2];
								u32 cellEntityIndex = entityLocation.cellEntityIndices[i2];
//								spatialGrid.grid[z][y][x].entities.Remove( cellEntityIndex );
								u32 lastElementIndex = 0;
								if( spatialGrid.grid[z][y][x].entities.GetSize() > 0 ) {
									lastElementIndex = spatialGrid.grid[z][y][x].entities.GetSize() - 1;
								}

//								std::cout << "last elem: " << lastElementIndex << std::endl;
								if( lastElementIndex > cellEntityIndex ) {       ///< Swap case
									const u32 swapedEntity = spatialGrid.grid[z][y][x].entities[lastElementIndex];
									spatialGrid.grid[z][y][x].entities[cellEntityIndex] = swapedEntity;
									spatialGrid.grid[z][y][x].entities.Remove( lastElementIndex );

									ecs::arch::EntityLocation& swapedEntityLocation = ecs::arch::world.entityLocations[ecs::arch::getId( swapedEntity )];
									for( u32 i3 = 0; i3 < swapedEntityLocation.gridCellCounter; ++i3 ) {
										const vec3 swapedEntityCurrentGridCell = swapedEntityLocation.gridCellIndicies[i3];
										if( swapedEntityCurrentGridCell == entityCurrentGridCell ) {
											swapedEntityLocation.cellEntityIndices[i3] = cellEntityIndex;
										}
									}
								} else {   ///< Remove last case
									spatialGrid.grid[z][y][x].entities.Remove( cellEntityIndex );
								}
							}
							entityLocation.gridCellCounter = 0;
//							entityLocation.isDirty         = true;     TODO: Have to make it work
						}

						archEntityManager->removeEntity( entity );
						arch::world.removeEntity( entity );
					}

					cm::font& fontComponent = componentsView.attackableFonts[i];
					fontComponent.font_string.clear();
					fontComponent.font_string.Push('4');
					fontComponent.font_string.Push('0');
					fontComponent.lifeTime = 0;
					fontComponent.removeble = true;
				}
			}
		}

		cachedFontArchetypesNumber = 0;
		arch::world.searchCacheArchetypes( fontRequiredMask, archView.cachedFontArchetypes, cachedFontArchetypesNumber );
		
		for( uint32_t x = 0; x < cachedFontArchetypesNumber; ++x ) {
			arch::Archetype* arch = archView.cachedFontArchetypes[x];
			componentsView.fonts = (ecs::components::font*)arch->components[arch::ComponentsIndices::FONT_COMPONENT];

			for ( unsigned int i = 0; i < arch->entityCount; ++i ) {
				if( componentsView.fonts ) {
					cm::font& fontComponent = componentsView.fonts[i];
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
