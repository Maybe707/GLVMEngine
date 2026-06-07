// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Systems/SpatialGridSystem.hpp"
#include "ArchetypeECS/ArchECS_World.hpp"
#include "Common/CommonFunctions.hpp"
#include "Vector.hpp"

namespace GLVM::ecs {
	void SpatialGridSystem::Update() {
		namespace arch = GLVM::ecs::arch;
		
		arch::SpatialGrid& spatialGrid = arch::world.spatialGrid;
		assert( spatialGrid.width > 0 && spatialGrid.height > 0 && spatialGrid.depth > 0 );
		const float chunkSize = spatialGrid.grid[0][0][0].size;

		const float halfWidth  = spatialGrid.width * chunkSize * 0.5f;
		const float halfHeight = spatialGrid.height * chunkSize * 0.5f;
		const float halfDepth  = spatialGrid.depth * chunkSize * 0.5f;

		cachedArchetypesNumber = 0;
		arch::world.searchCacheArchetypes( requiredMask, cachedArchetypes, cachedArchetypesNumber );
		
		for( uint32_t i0 = 0; i0 < cachedArchetypesNumber; ++i0 ) {
			arch::Archetype* arch = cachedArchetypes[i0];
			view.transforms = (ecs::components::transform*)arch->components[arch::ComponentsIndices::TRANSFORM_COMPONENT];
			view.meshes     = (ecs::components::mesh*)arch->components[arch::ComponentsIndices::MESH_COMPONENT];
			
			for( u32 i1 = 0; i1 < arch->entityCount; ++i1 ) {
				const components::transform& transform = view.transforms[i1];

				const components::mesh& mesh           = view.meshes[i1];

				components::MeshHandle entityMeshHandle = mesh.handle;
				core::MeshAxisMaxAbsoluteValues entityChunkBounds = allMeshMaxAbsoluteValues[entityMeshHandle.id];
				core::vector<vec3> entityBoxCornerBoundPoints = computeBoxCornerBoundPoints( entityChunkBounds, transform.position, transform.scale );

				for( u32 i2 = 0; i2 < entityBoxCornerBoundPoints.GetSize(); ++i2 ) {
					const vec3 entityPosition = entityBoxCornerBoundPoints[i2];
					
					const u32 index_X = (entityPosition[0] + halfWidth) / chunkSize;
					const u32 index_Y = (entityPosition[1] + halfHeight) / chunkSize;
					const u32 index_Z = (entityPosition[2] + halfDepth) / chunkSize;

					// if( arch::getId( arch->entities[i1] ) == 0 ) {
					// 	if( i2 == 0 ) {
					// 		std::cout << "0 iter" << std::endl;
					// 	}

					// 	std::cout << "entity position: " << transform.position << std::endl;
						
					// 	std::cout << "entity corner position: " << entityPosition << std::endl;
					// 	std::cout << "index x: " << index_X << std::endl;
					// 	std::cout << "index y: " << index_Y << std::endl;
					// 	std::cout << "index z: " << index_Z << std::endl;
					// }
					
					assert( index_X < spatialGrid.width && index_Y < spatialGrid.height && index_Z < spatialGrid.depth );
				
					const arch::entity entity = arch->entities[i1];
					core::vector<u32>& chunkEntities = spatialGrid.grid[index_Z][index_Y][index_X].entities;
					if( !core::isExist<u32>( chunkEntities, entity ) ) {
						chunkEntities.Push( entity );
					} else {
						continue;
					}
				}
			}
		}
	}
	
}; ///< namespace GLVM::core
