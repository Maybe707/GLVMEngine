#ifndef ARCHETYPE_INTERFACE_HPP
#define ARCHETYPE_INTERFACE_HPP

#include "ArchetypeECS/ArchECS_Utils.hpp"
#include "Vector.hpp"

namespace GLVM::ecs::arch {
	struct Archetype {
		core::vector<entity> entities;
		void* Components[ComponentsIndices::COMPONENTS_COUNT];
		componentMask mask;
	};
}; // namespace GLVM::ecs::arch

#endif
