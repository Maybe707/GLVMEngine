#ifndef ARCH_ECS_WORLD_HPP
#define ARCH_ECS_WORLD_HPP

#include "ArchetypeECS/ArchetypeInterface.hpp"
#include "ArchetypeECS/ArchECS_Utils.hpp"
#include "Vector.hpp"
#include "typenames.hpp"
#include <optional>
#include "ArchetypeECS/ArchetypeEntityManager.hpp"

namespace GLVM::ecs::arch {
	struct GridChunk {
		vec3 position;
		constexpr static float size = 32;
		core::vector<u32> entities;
        core::vector<u32> membershipIndices;
	};

	struct SpatialGrid {
		static const u32 width  = 32;
		static const u32 height = 32;
		static const u32 depth  = 32;
		GridChunk grid[width][height][depth];
        struct Range { u32 minX, minY, minZ, maxX, maxY, maxZ; };
        static std::optional<Range> cellRange(vec3 minimum, vec3 maximum);
	};
	
	struct World {
		World();
		~World();
        World(const World&) = delete;
        World& operator=(const World&) = delete;

		ArchetypeEntityManager entities;
		SpatialGrid spatialGrid;
		core::vector<Archetype*> archetypes;
		core::vector<EntityLocation> entityLocations;

		void addEntityToArchetype(entity entity_, Archetype* arch);
		void removeEntity(entity entity_);
        void detachSpatial(id value);
        void attachSpatial(id value, const SpatialGrid::Range& range);
        core::vector<Archetype*> query(componentMask requiredMask) const;
        Archetype* findArchetype(componentMask requiredMask) const;
        void searchCacheArchetypes(componentMask requiredMask, core::vector<Archetype*>& result, uint32_t& count) const;
        template<size_t N>
        void searchCacheArchetypes(componentMask requiredMask, Archetype* (&result)[N], uint32_t& count) const {
            const auto matches = query(requiredMask);
            if (matches.GetSize() > N) throw std::length_error("Archetype query buffer too small");
            count = matches.GetSize();
            std::copy(matches.begin(), matches.end(), result);
        }
	};


}; // namespace GLVM::ecs::arch


#endif
