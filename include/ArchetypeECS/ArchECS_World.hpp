#ifndef ARCH_ECS_WORLD_HPP
#define ARCH_ECS_WORLD_HPP

#include "ArchetypeECS/ArchetypeInterface.hpp"
#include "ArchetypeECS/ArchECS_Utils.hpp"
#include "Vector.hpp"
#include "typenames.hpp"

namespace GLVM::ecs::arch {
	struct GridChunk {
		vec3 position;
		constexpr static float size = 8;
		core::vector<u32> entities;
	};

	struct SpatialGrid {
		static const u32 width  = 64;
		static const u32 height = 64;
		static const u32 depth  = 64;
		GridChunk grid[width][height][depth];
	};
	
	struct World {
		World();
		~World();

		SpatialGrid spatialGrid;
		core::vector<Archetype*> archetypes;
		core::vector<EntityLocation> entityLocations;

		void addEntityToArchetype(entity entity_, Archetype* arch);
		void removeEntity(entity entity_);
		void searchCacheArchetypes( arch::componentMask requiredMask, arch::Archetype* cachedArchetypes[], uint32_t& cachedArchetypesNumber );
	};

	extern World world;
}; // namespace GLVM::ecs::arch


#endif
