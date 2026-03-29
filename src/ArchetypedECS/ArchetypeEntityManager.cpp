#include "ArchetypeECS/ArchetypeEntityManager.hpp"
#include <cstdint>

namespace GLVM::ecs::arch {
    ArchetypeEntityManager* ArchetypeEntityManager::pInstance_ = nullptr;
    std::mutex ArchetypeEntityManager::Mutex_;
    
    ArchetypeEntityManager::ArchetypeEntityManager() {}
    ArchetypeEntityManager::~ArchetypeEntityManager() {}
    
    ArchetypeEntityManager* ArchetypeEntityManager::getInstance()
    {
        std::lock_guard<std::mutex> lock(Mutex_);
        if(pInstance_ == nullptr)
        {
            pInstance_ = new ArchetypeEntityManager();
        }
        return pInstance_;
    }
	
	[[nodiscard]] entity ArchetypeEntityManager::createEntity() {
		id newId = 0;
        if( !freeList.empty() ) {   ///< Check out wether or not free ID in removed entities registry.
			newId = freeList.GetHead();
			freeList.Pop();
		} else {
			newId = nextId++;
			generations.Push( 1 );
		}
		
		return makeEntity( newId, generations[newId] );
	}

	void ArchetypeEntityManager::removeEntity( entity entity_ ) {
		id id_ = getId( entity_ );
		
		if ( !isAlive( entity_ ) ) return;
		
		generations[id_]++;
		freeList.Push( id_ );
    }

	bool ArchetypeEntityManager::isAlive( entity entity_ ) const {
		id id_ = getId( entity_ );

		return id_ < generations.GetSize() && generations[id_] == getGen( entity_ );
	}
}; // namespace GLVM::ecs::arch
