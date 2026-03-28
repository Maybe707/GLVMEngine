#ifndef ARCHETYPE_ENTITY_MANAGER
#define ARCHETYPE_ENTITY_MANAGER

#include <cstdint>
#include "Vector.hpp"
#include <mutex>

namespace GLVM::ecs::arch {
	typedef uint64_t entity;
	typedef uint32_t id;
	typedef uint32_t generation;
	
#define ENTITY_ID_BITS 32
#define GENERATION_BITS 32

	constexpr uint64_t entityBitsMask = (1ull << ENTITY_ID_BITS) - 1;
	
	struct ArchetypeEntityManager {
		inline static id nextId = 0;
		core::vector<generation> generations;
		core::vector<id>         freeList;

		ArchetypeEntityManager();
		static ArchetypeEntityManager* getInstance();
		
		entity     makeEntity( id id_, generation generation_ );
		id         getId( entity entity_ ) const;
		generation getGen( entity entity_ ) const;
		
		entity createEntity();
		void   removeEntity( entity entity_ );
		bool   isAlive( entity entity_ ) const;

	private:
		static ArchetypeEntityManager* pInstance_;
        static std::mutex  Mutex_;

		~ArchetypeEntityManager();
	};
}; // namespace GLVM::ecs::arch


#endif
