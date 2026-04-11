// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Systems/PhysicsSystem.hpp"
#include "ArchetypeECS/ArchECS_Types.hpp"
#include "Archetypes/ItemArchetype.hpp"
#include "ComponentManager.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ColliderFlagsComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "EntityManager.hpp"
#include "Event.hpp"
#include "Globals.hpp"
#include "VertexMath.hpp"
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
    /*! This update searching for refering to colliders entities and check their
     *  transform components for collision, and if collision detected check if
     *  backtracking entity had gravity component for call Gravity function.
     */
         
    void CPhysicsSystem::Update() 
    {
		namespace cm = GLVM::ecs::components;

		cachedArchetypesNumber = 0;
		for( uint32_t i = 0; i < arch::world.archetypes.GetSize(); ++i ) {
			arch::Archetype* arch = arch::world.archetypes[i];
			arch::componentMask requiredMask = (1ul << arch::ComponentsIndices::TRANSFORM_COMPONENT) |
				(1ul << arch::ComponentsIndices::MOVE_COMPONENT) |
				(1ul << arch::ComponentsIndices::RIGID_BODY_COMPONENT) |
				(1ul << arch::ComponentsIndices::COLLIDER_COMPONENT);

			if( (arch->mask & requiredMask) == requiredMask ) {
				cachedArchetypes[cachedArchetypesNumber] = arch;
				++cachedArchetypesNumber;
			}
		}

		for( uint32_t x = 0; x < cachedArchetypesNumber; ++x ) {
			arch::Archetype* arch = cachedArchetypes[x];
			components::transform*     transformsView    = nullptr;
			components::move*          movesView         = nullptr;
			components::rigidBody*     rigidBodiesView   = nullptr;
			components::colliderFlags* colliderFlagsView = nullptr;
			switch( arch->mask ) {
			case arch::playerComponentMask:
				transformsView    = static_cast<arch::PlayerArchetype*>( arch )->transforms;
				movesView         = static_cast<arch::PlayerArchetype*>( arch )->moves;
				rigidBodiesView   = static_cast<arch::PlayerArchetype*>( arch )->rigidBodies;
				colliderFlagsView = static_cast<arch::PlayerArchetype*>( arch )->colliderFlags;
				break;
			case arch::enemyComponentMask:
				transformsView    = static_cast<arch::EnemyArchetype*>( arch )->transforms;
				movesView         = static_cast<arch::EnemyArchetype*>( arch )->moves;
				rigidBodiesView   = static_cast<arch::EnemyArchetype*>( arch )->rigidBodies;
				colliderFlagsView = static_cast<arch::EnemyArchetype*>( arch )->colliderFlags;
				break;
			case arch::itemComponentMask:
				transformsView    = static_cast<arch::ItemArchetype*>( arch )->transforms;
				movesView         = static_cast<arch::ItemArchetype*>( arch )->moves;
				rigidBodiesView   = static_cast<arch::ItemArchetype*>( arch )->rigidBodies;
				colliderFlagsView = static_cast<arch::ItemArchetype*>( arch )->colliderFlags;
				break;
			}
		
			float deltaTime = 5.5f * fDelta_Time_;
			for(unsigned int i = 0; i < arch->entityCount; ++i) {
				if( transformsView && colliderFlagsView &&
					movesView && rigidBodiesView ) {
					cm::transform& transformComponent = transformsView[i];
					cm::move& move = movesView[i];
//				cm::collider& collider = collidersView[i];
					cm::colliderFlags& colliderFlags = colliderFlagsView[i];
					uint8_t isGroudCollisionMask = (0u << 0) | (1u << 1) | (0u << 2) | (0u << 3);
					if( colliderFlags.flags & isGroudCollisionMask ) {
						move.gravity = 0;
						transformComponent.gravityAccumulator = 0.0f;
					}
					uint8_t isWallCollisionMask = (1u << 0) | (0u << 1) | (0u << 2) | (0u << 3);
					if( colliderFlags.flags & isWallCollisionMask ) {
						move.frameMovement = 0;
						uint8_t wallCollisionTurnOffMask = (0u << 0) | (1u << 1) | (1u << 2) | (1u << 3);
						colliderFlags.flags &= wallCollisionTurnOffMask;
					}
					transformComponent.position += move.frameMovement;
					transformComponent.position += move.gravity;
					move.gravity       = 0.0f;
					move.frameMovement = 0.0f;
//				componentManager->RemoveComponent<cm::move>(entityRefMove);

					cm::rigidBody& rigidBody = rigidBodiesView[i];
					if ( rigidBody.jumpAccumulator > 0.0f ) {
						rigidBody.jumpAccumulator -= deltaTime;
						vec3 jump = vec3{ 0.0f, 5.0f, 0.0f } * deltaTime;
						transformComponent.position += jump;
					}
				}
			}
		}
    }
}

