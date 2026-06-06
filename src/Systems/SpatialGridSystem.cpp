#include "Systems/SpatialGridSystem.hpp"

namespace GLVM::ecs {
	void SpatialGridSystem::Update() {
		namespace arch = GLVM::ecs::arch;
		
		arch::SpatialGrid spatialGrid = arch::world.spatialGrid;
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
			
			for( u32 i1 = 0; i1 < arch->entityCount; ++i1 ) {
				const ecs::components::transform& transform = view.transforms[i1];
				const u32 index_X = (transform.position[0] + halfWidth) / chunkSize;
				const u32 index_Y = (transform.position[1] + halfHeight) / chunkSize;
				const u32 index_Z = (transform.position[2] + halfDepth) / chunkSize;

				assert( index_X < spatialGrid.width && index_Y < spatialGrid.height && index_Z < spatialGrid.depth );
				
				const arch::entity entity = arch->entities[i1];
				spatialGrid.grid[index_Z][index_Y][index_X].entities.Push( entity );
			}
		}
	}
	
}; ///< namespace GLVM::core
