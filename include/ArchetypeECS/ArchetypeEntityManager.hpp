#ifndef ARCHETYPE_ENTITY_MANAGER
#define ARCHETYPE_ENTITY_MANAGER

#include "ArchetypeECS/ArchECS_Utils.hpp"
#include <cstdint>
#include "Vector.hpp"
#include <mutex>

namespace GLVM::ecs::arch {

	
	struct ArchetypeEntityManager {
		ArchetypeEntityManager() = default;
        ArchetypeEntityManager(const ArchetypeEntityManager&) = delete;
        ArchetypeEntityManager& operator=(const ArchetypeEntityManager&) = delete;
		
		entity createEntity();
		void   removeEntity( entity entity_ );
		bool   isAlive( entity entity_ ) const;

	private:
        core::vector<generation> generations;
        core::vector<id> freeList;
        std::vector<bool> occupied;
	};
}; // namespace GLVM::ecs::arch


#endif
