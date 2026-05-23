#include "ArchetypeECS/ArchECS_World.hpp"

#include "Archetypes/CrosshairArchetype.hpp"
#include "Archetypes/InventoryArchetype.hpp"
#include "Archetypes/LevelChunkArchetype.hpp"
#include "Archetypes/RigidBodyArchetype.hpp"
#include "Archetypes/SpotLightArchetype.hpp"
#include "Archetypes/StaticMeshArchetype.hpp"
#include "Archetypes/ProjectileArchetype.hpp"
#include "Archetypes/ItemArchetype.hpp"
#include "Archetypes/DirectionalLightArchetype.hpp"
#include "Archetypes/PointLightArchetype.hpp"

namespace GLVM::ecs::arch {
	World world = {};

	World::~World() {
		/// TODO: Make this work with enams
		delete static_cast<LevelChunkArchetype*>(archetypes[0]);
		archetypes[0] = nullptr;
		delete static_cast<PlayerArchetype*>(archetypes[1]);
		archetypes[1] = nullptr;
		delete static_cast<EnemyArchetype*>(archetypes[2]);
		archetypes[2] = nullptr;
		delete static_cast<ProjectileArchetype*>(archetypes[3]);
		archetypes[3] = nullptr;
		delete static_cast<StaticMeshArchetype*>(archetypes[4]);
		archetypes[4] = nullptr;
		delete static_cast<CrosshairArchetype*>(archetypes[5]);
		archetypes[5] = nullptr;
		delete static_cast<InventoryArchetype*>(archetypes[6]);
		archetypes[6] = nullptr;
		delete static_cast<ItemArchetype*>(archetypes[7]);
		archetypes[7] = nullptr;
		delete static_cast<DirectionalLightArchetype*>(archetypes[8]);
		archetypes[8] = nullptr;
		delete static_cast<PointLightArchetype*>(archetypes[9]);
		archetypes[9] = nullptr;
		delete static_cast<SpotLightArchetype*>(archetypes[10]);
		archetypes[10] = nullptr;
	}
	
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

	void World::searchCacheArchetypes( arch::componentMask requiredMask, arch::Archetype* cachedArchetypes[], uint32_t& cachedArchetypesNumber ) {
		for( uint32_t i = 0; i < arch::world.archetypes.GetSize(); ++i ) {
			arch::Archetype* arch = arch::world.archetypes[i];

			if( (arch->mask & requiredMask) == requiredMask ) {
				cachedArchetypes[cachedArchetypesNumber] = arch;
				++cachedArchetypesNumber;
			}
		}
	}
}; // namespace GLVM::ecs::arch
