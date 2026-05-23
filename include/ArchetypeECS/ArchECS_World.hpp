#ifndef ARCH_ECS_WORLD_HPP
#define ARCH_ECS_WORLD_HPP

#include "ArchetypeECS/ArchetypeInterface.hpp"
#include "ArchetypeECS/ArchECS_Utils.hpp"
#include "Vector.hpp"

namespace GLVM::ecs::arch {
	struct World {
		~World();
		
		core::vector<Archetype*> archetypes;
		core::vector<EntityLocation> entityLocations;

		void addEntityToArchetype(entity entity_, Archetype* arch);
		void removeEntity(entity entity_);
		void searchCacheArchetypes( arch::componentMask requiredMask, arch::Archetype* cachedArchetypes[], uint32_t& cachedArchetypesNumber );
	};

	extern World world;
}; // namespace GLVM::ecs::arch


#endif
