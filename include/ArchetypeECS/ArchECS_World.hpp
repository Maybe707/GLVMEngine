#ifndef ARCH_ECS_WORLD_HPP
#define ARCH_ECS_WORLD_HPP

#include "ArchetypeECS/ArchetypeInterface.hpp"
#include "ArchetypeECS/ArchECS_Utils.hpp"
#include "Vector.hpp"
#include "typenames.hpp"

namespace GLVM::ecs::arch {
	struct GridChunk {
		vec3 position;
		float size = 8;
	};

	struct SpecialGrid {
		static const u32 width  = 2;
		static const u32 height = 2;
		static const u32 depth  = 2;
		GridChunk grid[width][height][depth];
	};
	
	struct World {
		World();
		~World();

		SpecialGrid spetialGrid;
		core::vector<Archetype*> archetypes;
		core::vector<EntityLocation> entityLocations;

		void addEntityToArchetype(entity entity_, Archetype* arch);
		void removeEntity(entity entity_);
		void searchCacheArchetypes( arch::componentMask requiredMask, arch::Archetype* cachedArchetypes[], uint32_t& cachedArchetypesNumber );
	};

	extern World world;
}; // namespace GLVM::ecs::arch


#endif
