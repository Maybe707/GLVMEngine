#include "ArchetypeECS/ArchECS_World.hpp"

namespace GLVM::ecs::arch {
	World world = {};
	
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
}; // namespace GLVM::ecs::arch
