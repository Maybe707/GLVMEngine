#ifndef ARCHETYPE_INTERFACE_HPP
#define ARCHETYPE_INTERFACE_HPP

#include "ArchetypeECS/ArchECS_Utils.hpp"
#include "Vector.hpp"

namespace GLVM::ecs::arch {
	struct Archetype {
		static constexpr uint32_t CAPACITY = 1024;
		
		entity entities[CAPACITY];
		uint32_t entityCount = 0;
		uint32_t componentIds[ComponentsIndices::COMPONENTS_COUNT];
		uint32_t componentCount = 0;
		void* components[ComponentsIndices::COMPONENTS_COUNT];
		componentMask mask;

		uint32_t addEntity( entity entity_ );
		entity removeEntity( uint32_t index );
	};

	struct EntityLocation {
		Archetype* arch;
		uint32_t   index;
	};
}; // namespace GLVM::ecs::arch

#endif
