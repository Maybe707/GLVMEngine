#include "ArchetypeECS/ArchECS_World.hpp"

#include "Archetypes/CrosshairArchetype.hpp"
#include "Archetypes/InventoryArchetype.hpp"
#include "Archetypes/LevelChunkArchetype.hpp"
#include "Archetypes/RigidBodyArchetype.hpp"
#include "Archetypes/SpotLightArchetype.hpp"
#include "Archetypes/StaticMeshArchetype.hpp"
#include "Archetypes/ProjectileArchetype.hpp"
#include "Archetypes/ItemArchetype.hpp"
#include "Archetypes/DirectionalLightArchetype.hpp"
#include "Archetypes/PointLightArchetype.hpp"

namespace GLVM::ecs::arch {
	World world = {};

	World::World() {
		assert( spetialGrid.width > 0 && spetialGrid.height > 0 && spetialGrid.depth > 0 );
		
		const float chunkSize       = spetialGrid.grid[0][0][0].size;
		const float halfWorldWidth  = spetialGrid.width * chunkSize * 0.5f;
		const float halfWorldHeight = spetialGrid.height * chunkSize * 0.5f;
		const float halfWorldDepth  = spetialGrid.depth * chunkSize * 0.5f;
		const float halfChunkSize   = chunkSize * 0.5f;
		const vec3 pivot = vec3( -halfWorldWidth + halfChunkSize, -halfWorldHeight + halfChunkSize, -halfWorldDepth + halfChunkSize );
		for( u32 i0 = 0; i0 < spetialGrid.depth; ++i0 ) {
			for( u32 i1 = 0; i1 < spetialGrid.height; ++i1 ) {
				for( u32 i2 = 0; i2 < spetialGrid.width; ++i2 ) {
					spetialGrid.grid[i0][i1][i2].position = vec3( i2 * chunkSize, i1 * chunkSize, i0 * chunkSize ) + pivot;
				}
			}
		}
	}
	
	World::~World() {
		for( unsigned int i = 0; i < archetypes.GetSize(); ++i ) {
			delete archetypes[i];
			archetypes[i] = nullptr;
		}
	}
	
	void World::addEntityToArchetype(entity entity_, Archetype* arch) {
        id id_ = getId(entity_);

        if (id_ >= entityLocations.GetSize())
            entityLocations.Resize(id_ + 1);

        EntityLocation& location = entityLocations[id_];

        // if (location.arch != nullptr) {
        //     assert(false && "Entity already assigned to archetype");
        // }

        uint32_t index = arch->addEntity(entity_);

        location.arch  = arch;
        location.index = index;
    }

	void World::removeEntity(entity entity_) {
        id id_ = getId(entity_);
        EntityLocation& location = entityLocations[id_];

        Archetype* arch = location.arch;
        uint32_t index  = location.index;

        entity moved = arch->removeEntity(index);

        if (moved != entity_) {
            id movedId = getId(moved);
            entityLocations[movedId].index = index;
        }

        location.arch = nullptr;
    }

	void World::searchCacheArchetypes( arch::componentMask requiredMask, arch::Archetype* cachedArchetypes[], uint32_t& cachedArchetypesNumber ) {
		for( uint32_t i = 0; i < arch::world.archetypes.GetSize(); ++i ) {
			arch::Archetype* arch = arch::world.archetypes[i];

			if( (arch->mask & requiredMask) == requiredMask ) {
				cachedArchetypes[cachedArchetypesNumber] = arch;
				++cachedArchetypesNumber;
			}
		}
	}
}; // namespace GLVM::ecs::arch
