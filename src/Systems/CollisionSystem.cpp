// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// This file is part of Game Loop Versatile Modules (GLVM)
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Systems/CollisionSystem.hpp"
#include "Archetypes/ItemArchetype.hpp"
#include "Archetypes/LevelChunkArchetype.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ColliderFlagsComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Vector.hpp"
#include "VertexMath.hpp"
#include "ArchetypeECS/ArchECS_Types.hpp"
#include "ArchetypeECS/ArchECS_Utils.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"
#include "Archetypes/EnemyArchetype.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "VertexMath.hpp"
#include <Systems/ProjectileSystem.hpp>
#include <cstdint>
#include <sys/types.h>
#include "ArchetypeECS/ArchECS_World.hpp"
#include "Archetypes/PlayerArchetype.hpp"
#include "Archetypes/ProjectileArchetype.hpp"
#include "Archetypes/StaticMeshArchetype.hpp"
#include "ArchetypeECS/ArchECS_Types.hpp"

namespace GLVM::ecs
{
	void CCollisionSystem::Update()
	{
		namespace arch = GLVM::ecs::arch;
		namespace cm   = GLVM::ecs::components;

//		const arch::SpatialGrid& spatialGrid = arch::world.spatialGrid;
		
		cachedArchetypesNumber = 0;
		arch::world.searchCacheArchetypes( requiredMask, cachedArchetypes, cachedArchetypesNumber );
			
		const float cameraSpeed = 5.5f * fDelta_Time_;
		/// Outer cycle on every archetype
		for( uint32_t x = 0; x < cachedArchetypesNumber; ++x ) {
			arch::Archetype* arch = cachedArchetypes[x];
			view.backtrackingTransforms    = (ecs::components::transform*)arch->components[arch::ComponentsIndices::TRANSFORM_COMPONENT];
			view.backtrackingColliders     = (ecs::components::collider*)arch->components[arch::ComponentsIndices::COLLIDER_COMPONENT];
			view.backtrackingColliderFlags = (ecs::components::colliderFlags*)arch->components[arch::ComponentsIndices::COLLIDER_FLAGS_COMPONENT];
			view.backtrackingMeshes        = (ecs::components::mesh*)arch->components[arch::ComponentsIndices::MESH_COMPONENT];

			for(unsigned int i = 0; i < arch->entityCount; ++i) {
				/// Count on every entity in current outer archetype
				uint32_t backtrackingEntityID = arch->entities[i];
				
				uint8_t groudCollisionTurnOffMask = (1u << 0) | (0u << 1) | (1u << 2) | (1u << 3);
				if( view.backtrackingColliderFlags && view.backtrackingColliders &&
					view.backtrackingMeshes && view.backtrackingTransforms ) {
				
					view.backtrackingColliderFlags[i].flags = view.backtrackingColliderFlags[i].flags & groudCollisionTurnOffMask;
					view.backtrackingColliders[i].colliders.clear();
					components::mesh backtrackinEntityMesh = view.backtrackingMeshes[i];
					components::MeshHandle backtrackingEntityMeshHandle = backtrackinEntityMesh.handle;
					components::transform* backtrackingTransformComponent = &view.backtrackingTransforms[i];
					vec3 backtrackingTransform = backtrackingTransformComponent->position;
					[[maybe_unused]] float backtrackingScale = backtrackingTransformComponent->scale;

					arch::componentMask requiredMask = (1ul << arch::ComponentsIndices::MOVE_COMPONENT);
					/// Check if outer current archetype has move component
					if ( arch::matchesRequiredMask( arch->mask, requiredMask) ) {
						view.backtrackingMove = (ecs::components::move*)arch->components[arch::ComponentsIndices::MOVE_COMPONENT];
						backtrackingTransform += Normalize(view.backtrackingMove[i].frameMovement) * cameraSpeed;
						backtrackingTransform += view.backtrackingMove[i].gravity;
					}

					/// Inner cycle on every archetype
					for( uint32_t i1 = 0; i1 < cachedArchetypesNumber; ++i1 ) {
						arch::Archetype* comparedArch = cachedArchetypes[i1];
						view.comparedTransforms = (cm::transform*)comparedArch->components[arch::ComponentsIndices::TRANSFORM_COMPONENT];
						view.comparedMeshes     = (cm::mesh*)comparedArch->components[arch::ComponentsIndices::MESH_COMPONENT];

						/// Count on every entity in current inner archetype
						for(unsigned int j = 0; j < comparedArch->entityCount; ++j) {
							/// Check for same entityID and iteration
							uint32_t comparedEntityID = comparedArch->entities[j];
							if( backtrackingEntityID == comparedEntityID && i == j ) {
								continue;
							}
							
							if( view.comparedMeshes && view.comparedTransforms ) {
								components::mesh comparedEntityMesh = view.comparedMeshes[j];
								components::MeshHandle comparedEntityMeshHandle = comparedEntityMesh.handle;
						
								components::transform* comparedTransformComponent = &view.comparedTransforms[j];
								vec3  comparedTransform = comparedTransformComponent->position;
								float comparedScale     = comparedTransformComponent->scale;

								arch::componentMask requiredMask = (1ul << arch::ComponentsIndices::MOVE_COMPONENT);
								vec3 gravityTest{};
								/// Check if inner current archetype has move component
								if ( arch::matchesRequiredMask( comparedArch->mask, requiredMask) ) {
									view.comparedMove = (cm::move*)comparedArch->components[arch::ComponentsIndices::MOVE_COMPONENT];

									if( view.comparedMove ) {
										comparedTransform += Normalize(view.comparedMove[j].frameMovement) * cameraSpeed;
										comparedTransform += view.comparedMove[j].gravity;
										gravityTest = view.comparedMove[j].gravity;
									}
								}

								bool boxColliderFlag = false;
								bool upperActorCheckFlag = false;

								core::MeshAxisMaxAbsoluteValues backtrackingMeshAxisMaxAbsoluteValues = allMeshMaxAbsoluteValues[backtrackingEntityMeshHandle.id];
								core::MeshAxisMaxAbsoluteValues comparedMeshAxisMaxAbsoluteValues     = allMeshMaxAbsoluteValues[comparedEntityMeshHandle.id];
								
								boxColliderFlag = core::BoxCollider(backtrackingTransform,
															  comparedTransform,
															  backtrackingScale,
															  comparedScale,
															  backtrackingMeshAxisMaxAbsoluteValues,
															  comparedMeshAxisMaxAbsoluteValues);

								if ( boxColliderFlag ) {
									upperActorCheckFlag = UpperActorCheck(backtrackingTransform,
																		  comparedTransform,
																		  backtrackingScale,
																		  comparedScale,
																		  backtrackingEntityMeshHandle,
																		  comparedEntityMeshHandle);
								}

								if(upperActorCheckFlag && boxColliderFlag) {
									uint8_t groudCollisionTurnOnMask = (0u << 0) | (1u << 1) | (0u << 2) | (0u << 3);
									view.backtrackingColliderFlags[i].flags = view.backtrackingColliderFlags[i].flags | groudCollisionTurnOnMask;
									view.backtrackingColliders[i].colliders.Push(comparedArch->entities[j]);
							
									continue;
								}
                    
								if(boxColliderFlag) {
									uint8_t wallCollisionTurnOnMask = (1u << 0) | (0u << 1) | (0u << 2) | (0u << 3);
									view.backtrackingColliderFlags[i].flags = view.backtrackingColliderFlags[i].flags | wallCollisionTurnOnMask;
									view.backtrackingColliders[i].colliders.Push(comparedArch->entities[j]);
							
									continue;
								}
							}
						}
					}
				}
			}
		}
		cachedArchetypesNumber = 0;
	}

    bool CCollisionSystem::UpperActorCheck(vec3 backtrackingPosition,
										   vec3 comparedPosition,
										   float backtrackingScale,
										   float comparedScale,
										   components::MeshHandle backtrackingMeshHandle,
										   components::MeshHandle comparedMeshHandle) {
		core::MeshAxisMaxAbsoluteValues backtrackingMeshAxisMaxAbsoluteValues = allMeshMaxAbsoluteValues[backtrackingMeshHandle.id];
		core::MeshAxisMaxAbsoluteValues comparedMeshAxisMaxAbsoluteValues     = allMeshMaxAbsoluteValues[comparedMeshHandle.id];

		constexpr float epsilon = 0.15f;
        if( backtrackingPosition[1] + backtrackingMeshAxisMaxAbsoluteValues.origin_offset_y -
			backtrackingMeshAxisMaxAbsoluteValues.absolute_y * backtrackingScale + epsilon >
			comparedPosition[1] + comparedMeshAxisMaxAbsoluteValues.origin_offset_y +
			comparedMeshAxisMaxAbsoluteValues.absolute_y * comparedScale ) {
            return true;
        }

        return false;
    }
}
