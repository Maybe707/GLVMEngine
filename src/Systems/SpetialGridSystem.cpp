#include "Systems/SpetialGridSystem.hpp"

namespace GLVM::ecs {
	void SpecialGridSystem::Update() {
		namespace arch = GLVM::ecs::arch;
		
		arch::SpecialGrid spetialGrid = arch::world.spetialGrid;
		assert( spetialGrid.width > 0 && spetialGrid.height > 0 && spetialGrid.depth > 0 );
		const float chunkSize = spetialGrid.grid[0][0][0].size;

		const float halfWidth  = spetialGrid.width * chunkSize * 0.5f;
		const float halfHeight = spetialGrid.height * chunkSize * 0.5f;
		const float halfDepth  = spetialGrid.depth * chunkSize * 0.5f;

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

				assert( index_X < spetialGrid.width && index_Y < spetialGrid.height && index_Z < spetialGrid.depth );
				
				const arch::entity entity = arch->entities[i1];
				spetialGrid.grid[index_Z][index_Y][index_X].entities.Push( entity );
			}
		}
	}
	
}; ///< namespace GLVM::core
