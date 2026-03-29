#ifndef ARCH_ECS_WORLD_HPP
#define ARCH_ECS_WORLD_HPP

#include "ArchetypeECS/ArchetypeInterface.hpp"
#include "Vector.hpp"

namespace GLVM::ecs::arch {
	struct World {
		core::vector<Archetype*> archetypes;
	};
}; // namespace GLVM::ecs::arch


#endif
